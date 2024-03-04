#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>

#include "image.h"

using namespace std;

// HW0 #3
// const Image& im: input image
// return the corresponding grayscale image
Image rgb_to_grayscale(const Image& im){
  assert(im.c == 3); // only accept RGB images
  Image gray(im.w,im.h,1); // create a new grayscale image (note: 1 channel)
  
  for(int i=0; i< im.w; ++i){
    for (int j=0; j<im.h; ++j){
      int pix = i + j*im.w;
      int pixR = pix;
      int pixG = pix + im.w * im.h;
      int pixB = pix + im.w * im.h * 2;
      gray.data[pix] = 0.299 * im.data[pixR] + 0.587 * im.data[pixG] + 0.114 * im.data[pixB];
    }
  }
  
  return gray;
}

// Example function that changes the color of a grayscale image
Image grayscale_to_rgb(const Image& im, float r, float g, float b){
  assert(im.c == 1);
  Image rgb(im.w,im.h,3);
  
  for(int q2=0;q2<im.h;q2++)for(int q1=0;q1<im.w;q1++){
    rgb(q1,q2,0)=r*im(q1,q2);
    rgb(q1,q2,1)=g*im(q1,q2);
    rgb(q1,q2,2)=b*im(q1,q2);
  }
  
  return rgb;
}

// HW0 #4
// Image& im: input image to be modified in-place
// int c: which channel to shift
// float v: how much to shift
void shift_image(Image& im, int c, float v){
  assert(c>=0 && c<im.c); // needs to be a valid channel
  
  for(int i=0; i< im.w; ++i){
    for (int j=0; j<im.h; ++j){
      int pixel_address = i + j*im.w + c*im.w*im.h;
      im.data[pixel_address] += v;
    }
  }
}

// HW0 #8
// Image& im: input image to be modified in-place
// int c: which channel to scale
// float v: how much to scale
void scale_image(Image& im, int c, float v){
  assert(c>=0 && c<im.c); // needs to be a valid channel
  
  for(int i=0; i< im.w; ++i){
    for (int j=0; j<im.h; ++j){
      int pixel_address = i + j*im.w + c*im.w*im.h;
      im.data[pixel_address] *= v;
    }
  }
}

// HW0 #5
// Image& im: input image to be modified in-place

const float& clamp_float(const float& val, const float& low, const float& high){
  return (val < low) ? low : (high < val) ? high : val;
}

void clamp_image(Image& im){
  // TODO: clamp all the pixels in all channel to be between 0 and 1
  for(int i=0; i<im.w*im.h*im.c; ++i){
    im.data[i] = clamp_float(im.data[i], 0, 1);
  }
}

// These might be handy
float max(float a, float b, float c){
  return max({a,b,c});
}

float min(float a, float b, float c){
  return min({a,b,c});
}


// HW0 #6
// Image& im: input image to be modified in-place
void rgb_to_hsv(Image& im){
  assert(im.c==3 && "only works for 3-channels images");
  
  float R, G, B;
  float H, S, V, C;

  for (int i=0; i<im.w; ++i){
    for (int j=0; j<im.h; ++j){
      R = im.data[i + j*im.w + im.w*im.h*0];
      G = im.data[i + j*im.w + im.w*im.h*1];
      B = im.data[i + j*im.w + im.w*im.h*2];

      V = max(R,G,B);
      C = V - min(R,G,B);
      S = (V != 0)? C/V : 0;

      if(C != 0){
        if(V == R) H = (G-B)/C;
        else if(V == G) H = ((B-R)/C) + 2;
        else H = ((R-G)/C) + 4;
        H = (H < 0) ? (H/6)+1 : (H/6);
      }
      else{
        H = 0;
      }

      im.data[i + j*im.w + im.w*im.h*0] = H;
      im.data[i + j*im.w + im.w*im.h*1] = S;
      im.data[i + j*im.w + im.w*im.h*2] = V;
    }
  }
  
}

// HW0 #7
// Image& im: input image to be modified in-place
void hsv_to_rgb(Image& im){
  assert(im.c==3 && "only works for 3-channels images");
  
  float R, G, B;
  float H, S, V, C;
  float Xpos, Xneg;
  float minv, maxv;
  int Hmod;
    
  // TODO: Convert all pixels from HSV format to RGB format
  for(int i = 0 ; i < im.w ; ++i){
    for(int j = 0 ; j < im.h ; ++j){
      H = im.data[i + j*im.w + im.w*im.h*0];
      S = im.data[i + j*im.w + im.w*im.h*1];
      V = im.data[i + j*im.w + im.w*im.h*2];

      C = S * V;
      maxv = V;
      minv = V-C;

      H *= 6;
      Xpos = C * (H - floor(H)) + minv;
      Xneg = C * (1 - (H - floor(H))) + minv;
      Hmod = int(H);

      switch(Hmod) {
        case 0:
          im.data[i + j*im.w + im.w*im.h*0] = V;
          im.data[i + j*im.w + im.w*im.h*1] = Xpos;
          im.data[i + j*im.w + im.w*im.h*2] = minv;
        break;
        case 1:
          im.data[i + j*im.w + im.w*im.h*0] = Xneg;
          im.data[i + j*im.w + im.w*im.h*1] = V;
          im.data[i + j*im.w + im.w*im.h*2] = minv;
        break;
        case 2:
          im.data[i + j*im.w + im.w*im.h*0] = minv;
          im.data[i + j*im.w + im.w*im.h*1] = V;
          im.data[i + j*im.w + im.w*im.h*2] = Xpos;
        break;
        case 3:
          im.data[i + j*im.w + im.w*im.h*0] = minv;
          im.data[i + j*im.w + im.w*im.h*1] = Xneg;
          im.data[i + j*im.w + im.w*im.h*2] = V;
        break;
        case 4:
          im.data[i + j*im.w + im.w*im.h*0] = Xpos;
          im.data[i + j*im.w + im.w*im.h*1] = minv;
          im.data[i + j*im.w + im.w*im.h*2] = V;
        break;
        case 5:
          im.data[i + j*im.w + im.w*im.h*0] = V;
          im.data[i + j*im.w + im.w*im.h*1] = minv;
          im.data[i + j*im.w + im.w*im.h*2] = Xneg;
        break;
        default:
          printf("Error in imageDataToChar() - Hmod value is not recognized.\n");
        break;
      }
    }
  }
}

// HW0 #9
// Image& im: input image to be modified in-place
void rgb_to_lch(Image& im){
  assert(im.c==3 && "only works for 3-channels images");
  
   for(int i = 0 ; i < im.w*im.h*im.c ; ++i){
      if(im.data[i] <= 0.04045) im.data[i] = im.data[i]/12.92;
      else im.data[i] = powf((im.data[i]+0.055)/1.055, 2.4); 
    }
    // cout << "sRGB => RGB:  " << im(0,0,0) << ' ' << im(0,0,1) << ' ' << im(0,0,2) << endl;

    // linear RGB => CIEXYZ
    float R, G, B;
    // float matrix[3][3] = {{0.41239080, 0.35758434, 0.18048079}, 
    //                       {0.21263901, 0.71516868, 0.07219232}, 
    //                       {0.01933082, 0.11919478, 0.95053215}};
    for(int i = 0 ; i < im.w ; ++i){
      for(int j = 0 ; j < im.h ; ++j){
        R = im.data[i + j*im.w + im.w*im.h*0];
        G = im.data[i + j*im.w + im.w*im.h*1];
        B = im.data[i + j*im.w + im.w*im.h*2];
        im.data[i + j*im.w + im.w*im.h*0] = R*0.41239080 + G*0.35758434 + B*0.18048079;
        im.data[i + j*im.w + im.w*im.h*1] = R*0.21263901 + G*0.71516868 + B*0.07219232;
        im.data[i + j*im.w + im.w*im.h*2] = R*0.01933082 + G*0.11919478 + B*0.95053215;
      }
    }
    // cout << "RGB => xyz:  " << im(0,0,0) << ' ' << im(0,0,1) << ' ' << im(0,0,2) << endl;


    // CIEXYZ => CIELUV
    float X, Y, Z;
    float L, u, v;
    for(int i = 0 ; i < im.w ; ++i){
      for(int j = 0 ; j < im.h ; ++j){
        X = im.data[i + j*im.w + im.w*im.h*0];
        Y = im.data[i + j*im.w + im.w*im.h*1];
        Z = im.data[i + j*im.w + im.w*im.h*2];

        if (Y <= powf(6.0/29.0, 3.0)) L =  powf(29.0/3.0, 3.0) * Y;
        else L = 116 * powf(Y, 1.0/3.0) - 16.0;

        u = 13 * L * (((4*X) / (X + 15*Y + 3*Z)) - 0.2009);
        v = 13 * L * (((9*Y) / (X + 15*Y + 3*Z)) - 0.4610);

        im.data[i + j*im.w + im.w*im.h*0] = L;
        im.data[i + j*im.w + im.w*im.h*1] = u;
        im.data[i + j*im.w + im.w*im.h*2] = v;
      }
    }
    // cout << "xyz => luv:  " << im(0,0,0) << ' ' << im(0,0,1) << ' ' << im(0,0,2) << endl;


    // CIELUV => CIELCH
    float C, H, S;
    for(int i = 0 ; i < im.w ; ++i){
      for(int j = 0 ; j < im.h ; ++j){
        L = im.data[i + j*im.w + im.w*im.h*0];
        u = im.data[i + j*im.w + im.w*im.h*1];
        v = im.data[i + j*im.w + im.w*im.h*2];
        im.data[i + j*im.w + im.w*im.h*0] = L;
        im.data[i + j*im.w + im.w*im.h*1] = sqrtf(powf(u, 2.0) + powf(v, 2.0));
        im.data[i + j*im.w + im.w*im.h*2] = atan2f(v, u);
      }
    }
}

// HW0 #9
// Image& im: input image to be modified in-place
void lch_to_rgb(Image& im){
  assert(im.c==3 && "only works for 3-channels images");
  
  float L, C, H;
  for(int i = 0 ; i < im.w ; ++i){
    for(int j = 0 ; j < im.h ; ++j){
      C = im.data[i + j*im.w + im.w*im.h*1];
      H = im.data[i + j*im.w + im.w*im.h*2];
      im.data[i + j*im.w + im.w*im.h*1] = C * cosf(H);
      im.data[i + j*im.w + im.w*im.h*2] = C * sinf(H);
    }
  }

    // CIELUV => CIEXYZ
  float up, vp, u, v;
  float X, Y, Z;
  for(int i = 0 ; i < im.w ; ++i){
    for(int j = 0 ; j < im.h ; ++j){
      if(im.data[i + j*im.w + im.w*im.h*0] == 0){
        X = Y = Z = 0.0;
      }
      else{
        L = im.data[i + j*im.w + im.w*im.h*0];
        u = im.data[i + j*im.w + im.w*im.h*1];
        v = im.data[i + j*im.w + im.w*im.h*2];
            
        up = u / (13*L) + 0.2009;
        vp = v / (13*L) + 0.4610;
        if(L <= 8) Y = L * powf((3.0/29.0), 3.0);
        else  Y = powf((L+16)/116.0, 3.0);
        X = Y * (9*up) / (4*vp);
        Z = Y * (12 - (3*up) - (20*vp)) / (4*vp);
      }

      im.data[i + j*im.w + im.w*im.h*0] = X;
      im.data[i + j*im.w + im.w*im.h*1] = Y;
      im.data[i + j*im.w + im.w*im.h*2] = Z;
    }
  }

  // CIEXYZ => linear RGB => RGB
  // float matrix[3][3] = {{3.24096994, -1.53738318, -0.49861076}, 
  //                       {-0.96924364, 1.8759675, 0.04155506}, 
  //                       {0.05563008, -0.20397696, 1.05697151}};
  for(int i = 0 ; i < im.w ; ++i){
    for(int j = 0 ; j < im.h ; ++j){
      X = im.data[i + j*im.w + im.w*im.h*0];
      Y = im.data[i + j*im.w + im.w*im.h*1];
      Z = im.data[i + j*im.w + im.w*im.h*2];
      im.data[i + j*im.w + im.w*im.h*0] = X*3.24096994 + Y*(-1.53738318) + Z*(-0.49861076);
      im.data[i + j*im.w + im.w*im.h*1] = X*(-0.96924364) + Y*1.8759675 + Z*0.04155506;
      im.data[i + j*im.w + im.w*im.h*2] = X*0.05563008 + Y*(-0.20397696) + Z*1.05697151;
      for(int k = 0 ; k < im.c ; ++k){
        int idx = i + j*im.w + k*im.w*im.h ;
        if(im.data[idx] <= 0.0031308) im.data[idx] = im.data[idx]*12.92;
        else im.data[idx] =  1.055 * powf(im.data[idx], 1.0/2.4) - 0.055;
      }
    }
  }
}

// Implementation of member functions
void Image::clamp(void) { clamp_image(*this); }
void Image::shift(int c, float v) { shift_image(*this,c,v); }
void Image::scale(int c, float v) { scale_image(*this,c,v); }

void Image::HSVtoRGB(void) { hsv_to_rgb(*this); }
void Image::RGBtoHSV(void) { rgb_to_hsv(*this); }
void Image::LCHtoRGB(void) { lch_to_rgb(*this); }
void Image::RGBtoLCH(void) { rgb_to_lch(*this); }
