#include <cmath>
#include <iostream>
#include "image.h"

using namespace std;

// HW1 #1
// float x,y: inexact coordinates
// int c: channel
// returns the nearest neighbor to pixel (x,y,c)
float Image::pixel_nearest(float x, float y, int c) const{
  // Since you are inside class Image you can
  // use the member function pixel(a,b,c)
  return clamped_pixel(round(x), round(y),c);
}

// HW1 #1
// float x,y: inexact coordinates
// int c: channel
// returns the bilinearly interpolated pixel (x,y,c)
float Image::pixel_bilinear(float x, float y, int c) const{
  // Since you are inside class Image you can
  // use the member function pixel(a,b,c)
  int left, right, top, bottom;
  left = floorf(x);
  right = ceilf(x);
  top = floorf(y);
  bottom = ceilf(y);

  float tl, tr, bl, br;
  tl = clamped_pixel(left, top, c);
  tr = clamped_pixel(right, top, c);
  bl = clamped_pixel(left, bottom, c);
  br = clamped_pixel(right, bottom, c);

  float tl_a, tr_a, bl_a, br_a;
  tl_a = (x-left) * (y-top);
  tr_a = (right-x) * (y-top);
  bl_a = (x-left) * (bottom - y);
  br_a = (right-x) * (bottom - y);
    
  return tl_a * br + tr_a * bl + bl_a * tr + br_a * tl;
}

// HW1 #1
// int w,h: size of new image
// const Image& im: input image
// return new Image of size (w,h,im.c)
Image nearest_resize(const Image& im, int w, int h){
  Image ret(w,h,im.c);
  
  float w_ratio = (float)im.w / w;
  float h_ratio = (float)im.h / h;
  float ref_x, ref_y ;

  for(int i = 0 ; i < w ; ++i){
    for(int j = 0 ; j < h ; ++j){
      for(int k = 0 ; k < im.c ; ++k){
        ref_x = i * w_ratio + 0.5 * w_ratio - 0.5;
        ref_y = j * h_ratio + 0.5 * h_ratio - 0.5; 
        ret.set_pixel(i, j, k, im.pixel_nearest(ref_x, ref_y, k));
      }
    }
  }
  return ret;
}


// HW1 #1
// int w,h: size of new image
// const Image& im: input image
// return new Image of size (w,h,im.c)
Image bilinear_resize(const Image& im, int w, int h){
  Image ret(w,h,im.c);
  
  float w_ratio = (float)im.w / w;
  float h_ratio = (float)im.h / h;
  float ref_x, ref_y ;

  for(int i = 0 ; i < w ; ++i){
    for(int j = 0 ; j < h ; ++j){
      for(int k = 0 ; k < im.c ; ++k){
        ref_x = i * w_ratio + 0.5 * w_ratio - 0.5;
        ref_y = j * h_ratio + 0.5 * h_ratio - 0.5; 
        ret.set_pixel(i, j, k, im.pixel_bilinear(ref_x, ref_y, k));
      }
    }
  }
  return ret;
}