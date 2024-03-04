#include "image.h"

const int& clamp(const int& val, const int& low, const int& high){
  return (val < low) ? low : (high < val) ? high : val;
}

// HW0 #1
// const Image& im: input image
// int x,y: pixel coordinates
// int ch: channel of interest
// returns the 0-based location of the pixel value in the data array
int pixel_address(const Image& im, int x, int y, int ch){
  // TODO: calculate and return the index
  return ch * im.w * im.h + y * im.w + x;
}

// HW0 #1
// const Image& im: input image
// int x,y,ch: pixel coordinates and channel of interest
// returns the value of the clamped pixel at channel ch
float get_clamped_pixel(const Image& im, int x, int y, int ch){
    x = clamp(x, 0, im.w-1);
    y = clamp(y, 0, im.h-1);
    ch = clamp(ch, 0, im.c-1);
    return im.data[pixel_address(im, x, y, ch)];
}

// HW0 #1
// Image& im: input image
// int x,y,ch: pixel coordinates and channel of interest
void set_pixel(Image& im, int x, int y, int c, float value){
  if(x >= 0 && x < im.w && y >= 0 && y < im.h && c >= 0 && c < im.c){
    im.data[pixel_address(im, x, y, c)] = value;
  }
}


// HW0 #2
// Copies an image
// Image& to: destination image
// const Image& from: source image
void copy_image(Image& to, const Image& from){
  // allocating data for the new image
  to.data=(float*)calloc(from.w*from.h*from.c,sizeof(float));
  to.c=from.c;
  to.w=from.w;
  to.h=from.h;
  to.data = from.data;
  memcpy(to.data, from.data, from.c*from.w*from.h*(sizeof(float)));
}
