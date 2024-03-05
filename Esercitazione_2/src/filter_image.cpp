#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"

#define M_PI 3.14159265358979323846

// HW1 #2.1
// Image& im: image to L1-normalize
void l1_normalize(Image &im) {
    float sum = 0;
    for(int c = 0 ; c < im.c ; ++c){
        sum = 0;
        for(int i = 0 ; i < im.h * im.w ; ++i){
            sum += im.data[i + c*im.h*im.w];
        }
        if(sum > 0){
            for(int i = 0 ; i < im.h * im.w ; ++i){
                im.data[i + c*im.h*im.w] /= sum ; 
            }
        }
        else{
            for(int i = 0 ; i < im.h * im.w ; ++i){
                im.data[i + c*im.h*im.w] = 1.0/(im.h*im.w) ; 
            }
        }
    }
}

// HW1 #2.1
// int w: size of filter
// returns the filter Image of size WxW
Image make_box_filter(int w) {
    assert(w % 2); // w needs to be odd
    Image ret(w,w,1);
    for (int i=0; i<w*w; ++i){
        ret.data[i] = 1.0/(w*w);
    }
    return ret;
}

// HW1 #2.2
// const Image&im: input image
// const Image& filter: filter to convolve with
// bool preserve: whether to preserve number of channels
// returns the convolved image
Image convolve_image(const Image &im, const Image &filter, bool preserve) {
    assert(filter.c == 1);
    Image ret(im.w, im.h, im.c);
    Image np_ret(im.w, im.h, 1);
    // This is the case when we need to use the function clamped_pixel(x,y,c).
    // Otherwise you'll have to manually check whether the filter goes out of bounds
    
    // TODO: Make sure you set the sizes of ret properly. Use ret=Image(w,h,c) to reset ret
    // TODO: Do the convolution operator
    for(int i = 0 ; i < im.w ; ++i){
      for(int j = 0 ; j < im.h ; ++j){
        float np_conv_val = 0;
        for(int k = 0 ; k < im.c ; ++k){
          float conv_val = 0;

          for(int x = 0 ; x < filter.w ; ++x){
            for(int y = 0 ; y < filter.h ; ++y){
              int nx = i + x - int(filter.w/2);
              int ny = j + y - int(filter.h/2);
              conv_val += im.clamped_pixel(nx,ny,k) * filter(x,y,0);
            }
          }
          if(!preserve) np_conv_val += conv_val;
          else ret.set_pixel(i,j,k,conv_val);
        }
        if(!preserve) np_ret.set_pixel(i,j,0,np_conv_val);
      }
    }

    if(!preserve) return np_ret;
    return ret;
}

// HW1 #2.2+ Fast convolution
// const Image&im: input image
// const Image& filter: filter to convolve with
// bool preserve: whether to preserve number of channels
// returns the convolved image
Image convolve_image_fast(const Image &im, const Image &filter, bool preserve) {
    assert(filter.c == 1);
    Image ret;
    // This is the case when we need to use the function clamped_pixel(x,y,c).
    // Otherwise you'll have to manually check whether the filter goes out of bounds

    // TODO: Make sure you set the sizes of ret properly. Use ret=Image(w,h,c) to reset ret
    // TODO: Do the fast convolution operator. Remember to use Eigen for matrix operations
    NOT_IMPLEMENTED();

    // Make sure to return ret and not im. This is just a placeholder
    return im;
}


// HW1 #2.3
// returns basic 3x3 high-pass filter
Image make_highpass_filter() {
    Image hp_filter(3,3,1);
    hp_filter.data[0] = 0; hp_filter.data[1] = -1; hp_filter.data[2] = 0;   
    hp_filter.data[3] = -1; hp_filter.data[4] = 4; hp_filter.data[5] = -1;
    hp_filter.data[6] = 0; hp_filter.data[7] = -1; hp_filter.data[8] = 0;
    return hp_filter; 
}

// HW1 #2.3
// returns basic 3x3 sharpen filter
Image make_sharpen_filter() {
    Image sp_filter(3,3,1);
    sp_filter.data[0] = 0; sp_filter.data[1] = -1; sp_filter.data[2] = 0;
    sp_filter.data[3] = -1; sp_filter.data[4] = 5; sp_filter.data[5] = -1;
    sp_filter.data[6] = 0; sp_filter.data[7] = -1; sp_filter.data[8] = 0;
    return sp_filter;
}

// HW1 #2.3
// returns basic 3x3 emboss filter
Image make_emboss_filter() {
    Image eb_filter(3,3,1);
    eb_filter.data[0] = -2; eb_filter.data[1] = -1; eb_filter.data[2] = 0;
    eb_filter.data[3] = -1; eb_filter.data[4] = 1; eb_filter.data[5] = 1;
    eb_filter.data[6] = 0; eb_filter.data[7] = 1; eb_filter.data[8] = 2;
    return eb_filter;
}

// HW1 #2.4
// float sigma: sigma for the gaussian filter
// returns basic gaussian filter
Image make_gaussian_filter(float sigma) {
    int kernel =  (int)ceilf(6 * sigma) ;
    if(!(kernel % 2)) ++kernel;
    
    Image gaussian_filter(kernel,kernel,1);
    
    for(int i = 0 ; i < kernel ; ++i){
      for(int j = 0 ; j < kernel ; ++j){
        float val = (1.0/(2*powf(sigma, 2.0)*M_PI)) * exp(-(powf(i-kernel/2, 2.0)+powf(j-kernel/2, 2.0))/(2*powf(sigma, 2.0)));
        gaussian_filter.set_pixel(i,j,0,val);
      }
    }

    l1_normalize(gaussian_filter);
    return gaussian_filter;
}


// HW1 #3
// const Image& a: input image
// const Image& b: input image
// returns their sum
Image add_image(const Image &a, const Image &b) {
    assert(a.w == b.w && a.h == b.h &&
           a.c == b.c); // assure images are the same size
    Image ret(a.w, a.h, a.c);
    for(int i = 0 ; i < a.w ; ++i){
      for(int j = 0 ; j < a.h ; ++j){
        for(int k = 0 ; k < a.c ; ++k){
          ret.set_pixel(i,j,k,a(i,j,k)+b(i,j,k));
        }
      }
    }
    
    return ret;
}

// HW1 #3
// const Image& a: input image
// const Image& b: input image
// returns their difference res=a-b
Image sub_image(const Image &a, const Image &b) {
    assert(a.w == b.w && a.h == b.h &&
           a.c == b.c); // assure images are the same size
    Image ret(a.w, a.h, a.c);
    for(int i = 0 ; i < a.w ; ++i){
      for(int j = 0 ; j < a.h ; ++j){
        for(int k = 0 ; k < a.c ; ++k){
          ret.set_pixel(i,j,k,a(i,j,k)-b(i,j,k));
        }
      }
    }
    
    return ret;
}

// HW1 #4.1
// returns basic GX filter
Image make_gx_filter() {
    // TODO: Implement the filter
    Image gx_filter(3,3,1);
    gx_filter.data[0] = -1; gx_filter.data[1] = 0; gx_filter.data[2] = 1;
    gx_filter.data[3] = -2; gx_filter.data[4] = 0; gx_filter.data[5] = 2;
    gx_filter.data[6] = -1; gx_filter.data[7] = 0; gx_filter.data[8] = 1;
    return gx_filter;
}

// HW1 #4.1
// returns basic GY filter
Image make_gy_filter() {
    // TODO: Implement the filter
    Image gy_filter(3,3,1);
    gy_filter.data[0] = -1; gy_filter.data[1] = -2; gy_filter.data[2] = -1;
    gy_filter.data[3] = 0; gy_filter.data[4] = 0; gy_filter.data[5] = 0;
    gy_filter.data[6] = 1; gy_filter.data[7] = 2; gy_filter.data[8] = 1;
    return gy_filter;
}

// HW1 #4.2
// Image& im: input image
void feature_normalize(Image &im) {
    assert(im.w * im.h); // assure we have non-empty image

    for(int c = 0 ; c < im.c ; ++c){
      float max_v = -INFINITY;
      float min_v = INFINITY;
      for(int i = 0 ; i < im.w ; ++i){
        for(int j = 0 ; j < im.h ; ++j){
          if(im(i,j,c)>max_v) max_v = im(i,j,c);
          if(im(i,j,c)<min_v) min_v = im(i,j,c);
        }
      }
      if(max_v != min_v){
        float diff = max_v - min_v;
        for(int i = 0 ; i < im.w ; ++i){
          for(int j = 0 ; j < im.h ; ++j){
            float ori_v = im(i,j,c);
            im.set_pixel(i,j,c,(ori_v-min_v)/diff);
          }
        }
      }
      else{
        for(int i = 0 ; i < im.w ; ++i){
          for(int j = 0 ; j < im.h ; ++j){
            im.set_pixel(i,j,c,0);
          }
        }
      }
    }
}


// Normalizes features across all channels
void feature_normalize_total(Image &im) {
    assert(im.w * im.h * im.c); // assure we have non-empty image

    int nc = im.c;
    im.c = 1;
    im.w *= nc;

    feature_normalize(im);

    im.w /= nc;
    im.c = nc;

}


// HW1 #4.3
// Image& im: input image
// return a pair of images of the same size
pair<Image, Image> sobel_image(const Image &im) {
    Image gx_filter = make_gx_filter();
    Image gy_filter = make_gy_filter();

    Image gx = convolve_image(im, gx_filter, false);
    Image gy = convolve_image(im, gy_filter, false);

    Image mag(im.w, im.h, 1);
    Image theta(im.w, im.h, 1);

    for(int i = 0 ; i < im.w ; ++i){
      for(int j = 0 ; j < im.h ; ++j){
        mag.set_pixel(i, j, 0, sqrtf(powf(gx(i,j,0),2.0) + powf(gy(i,j,0),2.0)));
        theta.set_pixel(i, j, 0,atan2(gy(i,j,0), gx(i,j,0)));
      }
    }

    return {mag, theta};
}


// HW1 #4.4
// const Image& im: input image
// returns the colorized Sobel image of the same size
Image colorize_sobel(const Image &im) {
    // TODO: Your code here
    pair<Image,Image> sobel_im = sobel_image(im);
    Image mag = sobel_im.first;
    Image theta = sobel_im.second;

    feature_normalize(mag);

    for(int i = 0 ; i < theta.w ; ++i){
      for(int j = 0 ; j < theta.h ; ++j){
        float ori_v = theta(i,j,0);
        theta.set_pixel(i,j,0,ori_v/(2*M_PI)+0.5);
      }
    }

    Image ret(im.w, im.h, 3);
    for(int i = 0 ; i < im.w ; ++i){
      for(int j = 0 ; j < im.h ; ++j){
        ret.set_pixel(i,j,0,theta(i,j,0));
        ret.set_pixel(i,j,1,mag(i,j,0));
        ret.set_pixel(i,j,2,mag(i,j,0));
      }
    }
    
    hsv_to_rgb(ret);
    Image f = make_gaussian_filter(4);
    return convolve_image(ret, f, true);

}

float gaussian(float x,float sigma){
  return (2*M_PI*powf(sigma, 2.0)) * exp(-(powf(x, 2.0))/(2*powf(sigma, 2.0)));
}

// HW1 #4.5
// const Image& im: input image
// float sigma1,sigma2: the two sigmas for bilateral filter
// returns the result of applying bilateral filtering to im
Image bilateral_filter(const Image &im, float sigma1, float sigma2) {
    Image ret(im.w, im.h, im.c);
    
    // Bilateral filter's kernel size
    int kernel =  (int)ceilf(6 * sigma1) ;
    if(!(kernel % 2)) ++kernel;
    
    // cout << kernel << endl;

    // TODO: Your bilateral code
    // Spatial Gaussian
    Image gsf = make_gaussian_filter(sigma1); 

    for(int i = 0 ; i < im.w ; ++i){
      for(int j = 0 ; j < im.h ; ++j){
        for(int k = 0 ; k < im.c ; ++k){
          float sum = 0 ;
          // Caculate normalization factor N
          for(int x = 0 ; x < kernel ; ++x){
            for(int y = 0 ; y < kernel ; ++y){
              float current_pix_v = im(i, j, k);
              float neighbor_pix_v = im.clamped_pixel(i+x-int(kernel/2), j+y-int(kernel/2), k);
              float cur_gs = gsf(x,y,0);
              float cur_gc = gaussian((current_pix_v-neighbor_pix_v), sigma2);
              sum += cur_gs * cur_gc;
            }
          }
          float new_pix_v = 0 ;
          // Filter image with bilateral filter W
          for(int x = 0 ; x < kernel ; ++x){
            for(int y = 0 ; y < kernel ; ++y){
              float current_pix_v = im(i, j, k);
              float neighbor_pix_v = im.clamped_pixel(i+x-(kernel/2), j+y-(kernel/2), k);
              float cur_gs = gsf(x,y,0);
              float cur_gc = gaussian((current_pix_v-neighbor_pix_v), sigma2);
              new_pix_v += im.clamped_pixel(i+x-int(kernel/2), j+y-int(kernel/2), k) * cur_gs * cur_gc / sum ;
            }
          }
          ret.set_pixel(i,j,k,new_pix_v);
        }
      }
    }

    return ret;
}

// HW1 #4.5+ Fast bilateral filter
// const Image& im: input image
// float sigma1,sigma2: the two sigmas for bilateral filter
// returns the result of applying bilateral filtering to im
Image bilateral_filter_fast(const Image &im, float sigma1, float sigma2) {
    Image bf = im;

    // TODO: Your fast bilateral code
    NOT_IMPLEMENTED();

    return bf;
}

// HM #5
//
float *compute_histogram(const Image &im, int ch, int num_bins) {
    float *hist = (float *) malloc(sizeof(float) * num_bins);
    for (int i = 0; i < num_bins; ++i) {
        hist[i] = 0;
    }

    // TODO: Your histogram code
    NOT_IMPLEMENTED();

    return hist;
}

float *compute_CDF(float *hist, int num_bins) {
    float *cdf = (float *) malloc(sizeof(float) * num_bins);

    cdf[0] = hist[0];

    // TODO: Your cdf code
    NOT_IMPLEMENTED();

    return cdf;
}

Image histogram_equalization_hsv(const Image &im, int num_bins) {
    Image new_im(im);
    float eps = 1.0 / (num_bins * 1000);

    // TODO: Your histogram equalization code
    NOT_IMPLEMENTED();
    // convert to hsv
    // compute histograms for the luminance channel
    // compute cdf
    // equalization
    // convert back to rgb

    // delete the allocated memory!
//    delete hist;
//    delete cdf;

    return new_im;
}

Image histogram_equalization_rgb(const Image &im, int num_bins) {
    Image new_im(im);
    float eps = 1.0 / (num_bins * 1000);

    // compute histograms for each color channel
    for (int c = 0; c < im.c; ++c) {

        // TODO: Your equalization code
        NOT_IMPLEMENTED();

        // delete the allocated memory!
//        delete hist;
//        delete cdf;
    }

    return new_im;
}


// HELPER MEMBER FXNS

void Image::feature_normalize(void) { ::feature_normalize(*this); }

void Image::feature_normalize_total(void) { ::feature_normalize_total(*this); }

void Image::l1_normalize(void) { ::l1_normalize(*this); }

Image operator-(const Image &a, const Image &b) { return sub_image(a, b); }

Image operator+(const Image &a, const Image &b) { return add_image(a, b); }
