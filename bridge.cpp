#include "bridge.h"
//#include "led-matrix.h" // Hardware-specific library
//#include "graphics.h"
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

using namespace rgb_matrix;
using namespace std;


int random(int low, int high)
{
    trng<unsigned int> rng_int;
    return(rng_int.random(low, high));
}
