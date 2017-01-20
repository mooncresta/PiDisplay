#include "PiDisplay.h"

#ifdef PANEL_MESSAGE


// Globals
rgb_matrix::Color p  = Color(255,255,255);
FrameCanvas *offscreen;

  // Scroll Text with "scroll_jumps" pixels every "scroll_ms" milliseconds.
  // If "scroll_ms" is negative, don't do any scrolling.

MessageScroller::MessageScroller(RGBMatrix *m, int scroll_jumps, int scroll_ms = 30, Color(m_color) = Color(255,255,255))
    : ThreadedCanvasManipulator(m), scroll_jumps_(scroll_jumps),
      scroll_ms_(scroll_ms),
      horizontal_position_(0),
      matrix_(m) {
      offscreen = matrix_->CreateFrameCanvas();
    DEBUG("In scroll Constructor\n");
    p = m_color;
}

MessageScroller::~MessageScroller() {
    Stop();
    WaitStopped();   // only now it is safe to delete our instance variables.
}

void MessageScroller::Run() {

//    char *m = &g_scrolling_message[0];

    DEBUG("In scroll Run\n");

    while (running() && !interrupt_received) {

        int l = (strlen(g_scrolling_message)*-6) - K_MS_WIDTH;
        for(int i = K_MS_WIDTH; i > l; i--){
		// Clear the area
                DEBUG("In scroll loop\n");
                fillRect(K_MS_X, K_MS_Y-1, K_MS_WIDTH, K_MS_HEIGHT, Color(0,0,0));
                drawString(i,K_MS_Y, &g_scrolling_message[0], F_LARGE, Color(255,0,0));
                swapBuffers(false);
                usleep(3 * 10000);
        }

      }

/*
        usleep(100 * 1000);
      for (int x = 0; x < screen_width; ++x) {
        for (int y = 0; y < screen_height; ++y) {

//          const Pixel &p = current_image_.getPixel((horizontal_position_ + x) % current_image_.width, y);
          offscreen_->SetPixel(x, y, p.r, p.g, p.b);
        }
      }

//      offscreen = SwapOnVSync(offscreen);
	swapBuffers(false);

      int horizontal_position = 0;
      horizontal_position += scroll_jumps_;
      if (horizontal_position_ < 0) horizontal_position_ = K_MS_WIDTH;
      if (scroll_ms_ <= 0) {
        // No scrolling. We don't need the image anymore.
//        current_image_.Delete();
      } else {
        usleep(scroll_ms_ * 1000);
      }
*/
}


#endif  //MESSAGE
