#!/usr/bin/python

###########################################################
##  Script for sending text to Adafruit LED Matrices
##
##  Author: Micah Hoffman - @webbreacher
##
###########################################################

####
# Load libraries
####
from PIL import Image, ImageDraw, ImageFont
from rgbmatrix import Adafruit_RGBmatrix
from datetime import datetime, timedelta
from pytz import timezone
import time
import pytz
import requests
import json

####
# Variables
####
utc = pytz.utc
text = ''
all_food_trucks_combined = set()
my_zip = 22201   # US Zip Code used in the weather module
my_lat = 38.92   # The latitude you want to use as a location. More numbers = more precise & fewer results
my_lon = -77.17  # The longitude you want to use as a location
ua = 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.93 Safari/537.36'

####
# Methods
####
def MatrixFill(rgb, speed = 0.5):
    # The passed RGB param should be in hex (0xFF0000) format
    matrix.Fill(rgb)
    time.sleep(speed)
    matrix.Clear()

def ImageToMatrixScrollHor(image, direction, speed = 0.05):
    # This is what takes the Image and sends it to the Matrix
    # It scrolls horizontally
    matrix.Clear()
    for n in range(32, -image.size[0], -1): # To switch from R2L to L2R,change 1st 2 params
        if direction == 'l2r':
            matrix.SetImage(image.im.id, -n, 1)
        else:
            matrix.SetImage(image.im.id, n, 1)
        time.sleep(speed) # This controls how fast it scrolls. The longer the delay the slower.
    matrix.Clear()

def ImageToMatrixScrollVer(image, direction, speed = 0.05):
    # This is what takes the Image and sends it to the Matrix
    # It scrolls vertically
    matrix.Clear()
    for n in range(-33, 32): # Start off top-left, move off bottom-right
        # IMPORTANT: *MUST* pass image ID, *NOT* image object!
        if direction == 'up':
            matrix.SetImage(image.im.id, 1, -n) # -n goes up in last param
        else:
            matrix.SetImage(image.im.id, 1, n) # n in last param goes down
        if n==0:
            time.sleep(2) # This pauses the display when the content fills the matrix at 0 Y
        else:
            time.sleep(speed) # This controls how fast it scrolls. The longer the delay the slower.
    matrix.Clear()

def CreateImage(text, textwidth = 2200, imgwidth = 2200, textColor = (0, 0, 255)):
    # This creates an image that is then passed to the matrix
    # TODO - Dynamically set the image and text widths according
    height = 32
    #textBackgroundColor = (255, 0, 0) # RGB
    textX = textwidth  # text width in pixels
    textY = height # text height in pixels
    textTopLeftX = 0
    textTopLeftY = 0

    ####
    # Create new image
    ####
    # TODO - Need to dynamically adjust the pixel size of the image
    imgx = imgwidth  # image width in pixels
    imgy = height # image height in pixels
    image = Image.new("RGB", (imgx, imgy))

    # Commands to load an load image from the file system
    #image = Image.open("input.png")
    #(imgx, imgy) = image.size
    #image = image.resize((imgx, imgy), Image.BICUBIC)

    font = ImageFont.truetype("/usr/share/fonts/truetype/roboto/Roboto-Regular.ttf", 48)
    (width, height) = font.getsize(text)
    textImage = font.getmask(text)
    pixels = image.load()
    for y in range(imgy):
        by = int(height * (y - textTopLeftY) / textY + 0.5)
        if by >= 0 and by < height:
            for x in range(imgx):
                bx = int(width * (x - textTopLeftX) / textX + 0.5)
                if bx >= 0 and bx < width:
                    if textImage.getpixel((bx, by)) == 0: # text background
                        pass # transparent background
                        #pixels[x, y] = textBackgroundColor
                        #(r, g, b) = pixels[x, y]
                    else: # text foreground
                        pixels[x, y] = textColor
                        #(r, g, b) = pixels[x, y]
    return image

def TimeZoneText():
    # Create the 'text' string with all the time zones
    global text
    text = ''
    # Got these from https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
    zones = {'Beijing': 'Asia/Shanghai',
             'Hawaii': 'Pacific/Honolulu',
             'Los Angeles': 'America/Los_Angeles',
             'Denver': 'America/Denver',
             'Chicago': 'America/Chicago',
             'New York': 'America/New_York',
             'UTC': 'UTC',
             'Moscow': 'Europe/Moscow',
             'Dubai': 'Asia/Dubai',
             'Lebanon': 'Africa/Cairo'}
    fmt = '%H:%M' # Time display format
    #text = time.asctime( time.localtime(time.time()) ) # Shows localtime
    for city, zone in sorted(zones.iteritems()):
        zone_time = datetime.now(timezone(zone))
        text = text + '      ' + city + ' -> ' + zone_time.strftime(fmt)
    return text

def MetroTrainInfo():
    # This function accesses the Washington DC Area Metro Transit Authority train information
    # You will need to get an API key from their site before using this.
    requests.packages.urllib3.disable_warnings()
    headers = {'api_key': '[APIKEY]', 'User-Agent': ua} # Get this from the API area of WMATA's site
    wmata = requests.get("https://api.wmata.com/StationPrediction.svc/json/GetPrediction/ N03", headers=headers, verify=False)
    #print json.dumps(wmata.json(), indent=4)
    trains = wmata.json()
    all_trains=""
    for train in trains['Trains']:
        if train['Min']=='ARR' or train['Min']=='BRD':
            state='NOW'
        else:
            state='%s mins' % train['Min']
        if train['DestinationName']=='No Passenger' or train['Car']=='-':
            continue
        all_trains = all_trains+'%s going to %s --- ' % (state, train['DestinationName'])
    all_trains_lst=[]
    all_trains_lst=all_trains.strip(" --- "),str(len(all_trains))
    return all_trains_lst

def FoodTrucks():
    precision = len(str(my_lat))
    headers = {'User-Agent': ua}
    try:
        food_trucks = requests.get('http://foodtruckfiesta.com/apps/map_json.php', headers=headers)
        all_food_trucks = food_trucks.json()
        for truck in all_food_trucks['markers']:
            if str(truck['coord_lat'])[:precision] == str(my_lat):
                all_food_trucks_combined.add(truck['print_name'])
        food_trucks_out =  ', '.join(sorted(all_food_trucks_combined))
        if food_trucks_out == '':
            food_trucks_out = '- No food trucks found -'
        return food_trucks_out
    except:
        return '* Food Truck ERROR - Web Call *'

def WeatherUpdate():
    headers = {'User-Agent': ua}
    url = 'http://api-ak.wunderground.com/api/c991975b7f4186c0/forecast/v:2.0/q/zmw:'+str(my_zip)+'.1.99999.json'
    try:
        weather_req = requests.get(url, headers=headers)
        all_weather = weather_req.json()
        if 'error' in all_weather['response']:
            print 'ERROR! %s' % all_weather['response']['error']['description']
            return '* Weather ERROR *'
        else:
            weather_out = 'DAY: %s; NIGHT: %s' % (all_weather['forecast']['days'][0]['summary']['day']['text'], all_weather['forecast']['days'][0]['summary']['night']['text'])
            return weather_out
    except:
        return '* Weather ERROR - Web Call *'
    
####
# Create
####
matrix = Adafruit_RGBmatrix(32, 4)

####
# Call the Function(s) to create content and write this to the Matrix
####
while True: 
    #MatrixFill(0x00AA00, 1) #Fill the matrix with a single color
    #image = CreateImage('-> NoVA Hackers <-', 124, 124, (180, 0, 0))
    #ImageToMatrixScrollVer(image, 'down', 0.02)
    image = CreateImage('Welcome to the NoVA Hackers meeting.', 400, 400, (0, 0, 255)) #Blue
    ImageToMatrixScrollHor(image, 'r2l', 0.01)

    image = CreateImage('Today's Weather', 124, 124, (180, 180, 180))
    ImageToMatrixScrollVer(image, 'up', 0.02)
    image = CreateImage(WeatherUpdate(), len(WeatherUpdate())*7, len(WeatherUpdate())*7, (0, 180, 180)) #???
    ImageToMatrixScrollHor(image, 'r2l', 0.01)
    
    image = CreateImage('Food Trucks', 124, 124, (180, 180, 180))
    ImageToMatrixScrollVer(image, 'up', 0.02)
    image = CreateImage(FoodTrucks(), len(FoodTrucks())*7, len(FoodTrucks())*7, (180, 0, 0)) #Red
    ImageToMatrixScrollHor(image, 'r2l', 0.01)
    
    image = CreateImage('Greensboro Metro', 124, 124, (180, 180, 180))
    ImageToMatrixScrollVer(image, 'up', 0.02)
    metro = MetroTrainInfo()
    image = CreateImage(metro[0], int(metro[1])*9, int(metro[1])*9, (0, 180, 0)) # Green
    ImageToMatrixScrollHor(image, 'r2l', 0.01)
    
    image = CreateImage('World Time', 124, 124, (180, 180, 180))
    ImageToMatrixScrollVer(image, 'up', 0.02)
    image = CreateImage(TimeZoneText(), 2300, 2300, (180, 0, 180)) #Purple
    ImageToMatrixScrollHor(image, 'r2l', 0.01)
