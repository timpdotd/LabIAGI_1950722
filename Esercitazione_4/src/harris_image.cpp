#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cassert>

#include "image.h"
//#include "matrix.h"

using namespace std;


// Create a feature descriptor for an index in an image.
// const Image& im: source image.
// int x,y: coordinates for the pixel we want to describe.
// returns: Descriptor for that index.
Descriptor describe_index(const Image& im, int x, int y, int w){
  Descriptor d;
  d.p={(double)x,(double)y};
  d.data.reserve(w*w*im.c);
  
  // If you want you can experiment with other descriptors
  // This subtracts the central value from neighbors
  // to compensate some for exposure/lighting changes.
  for(int c=0;c<im.c;c++){
    float cval = im.clamped_pixel(x,y,c);
    for(int dx=-w/2;dx<=w/2;dx++)for(int dy=-w/2;dy<=w/2;dy++){
      d.data.push_back(im.clamped_pixel(x+dx,y+dy,c)-cval);
    }
  }
  return d;
}

// Marks the spot of a point in an image.
// Image& im: image to mark.
// Point p: spot to mark in the image.
void mark_spot(Image& im, const Point& p){
  int x = p.x;
  int y = p.y;
  
  for(int i = -9; i < 10; ++i){
    im.set_pixel(x+i, y, 0, 1);
    im.set_pixel(x, y+i, 0, 1);
    im.set_pixel(x+i, y, 1, 0);
    im.set_pixel(x, y+i, 1, 0);
    im.set_pixel(x+i, y, 2, 1);
    im.set_pixel(x, y+i, 2, 1);
  }
}

// Marks corners denoted by an array of descriptors.
// Image& im: image to mark.
// vector<Descriptor> d: corners in the image.
Image mark_corners(const Image& im, const vector<Descriptor>& d){
  Image im2=im;
  for(auto&e1:d)mark_spot(im2,e1.p);
  return im2;
}

// HW5 1.1b
// Creates a 1d Gaussian filter.
// float sigma: standard deviation of Gaussian.
// returns: single row Image of the filter.
Image make_1d_gaussian(float sigma){
  int dimension = ceil(6 * sigma);
  if (dimension % 2 == 0) dimension++;

  Image im(dimension, 1);
  float multiplier = 1.0 / (sqrt(2.0 * M_PI) * sigma);
  for (int x = 0; x < dimension; x++) {
    float k = x - (dimension / 2);
    float exponent = -1.0 * pow(k, 2) / (2 * pow(sigma, 2));
    im(x, 0, 0) = multiplier * exp(exponent);
  }
  return im;
}

// HW5 1.1b
// Smooths an image using separable Gaussian filter.
// const Image& im: image to smooth.
// float sigma: std dev. for Gaussian.
// returns: smoothed Image.
Image smooth_image(const Image& im, float sigma){
  Image filter = make_1d_gaussian(sigma);
  Image im1 = convolve_image(im, filter, true);
  swap(filter.h, filter.w);
  Image im2 = convolve_image(im1, filter, true);
  return im2;
}

// HW5 1.1
// Calculate the structure matrix of an image.
// const Image& im im: the input image.
// float sigma: std dev. to use for weighted sum.
// returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
//          third channel is IxIy.
Image structure_matrix(const Image& im2, float sigma){
  TIME(1);
  // only grayscale or rgb
  assert((im2.c==1 || im2.c==3) && "only grayscale or rgb supported");
  Image im;
  // convert to grayscale if necessary
  if(im2.c==1)im=im2;
  else im=rgb_to_grayscale(im2);
  
  Image S(im.w, im.h, 3);
  Image Ix_im = convolve_image(im, make_gx_filter(), false);
  Image Iy_im = convolve_image(im, make_gy_filter(), false);
  
  //Passa le derivate di Ix e Iy in due matrici
  Matrix Ix(im.w, im.h);
  Matrix Iy(im.w, im.h);
  for (int w = 0; w < im.w; w++){
    for (int h = 0; h < im.h; h++){
      Ix(w, h) = Ix_im(w, h, 0);
      Iy(w, h) = Iy_im(w, h, 0);
    }
  }
  Matrix IxIx = elementwise_multiply(Ix, Ix); // Misura l'intensità del gradiente lungo l'asse x
  Matrix IyIy = elementwise_multiply(Iy, Iy); // Misura l'intensità del gradiente lungo l'asse y
  Matrix IxIy = elementwise_multiply(Ix, Iy); // Misura la correlazione tra i gradienti lungo gli assi x e y

  for (int w = 0; w < im.w; w++){
    for (int h = 0; h < im.h; h++) {
      S(w, h, 0) = IxIx(w, h);
      S(w, h, 1) = IyIy(w, h);
      S(w, h, 2) = IxIy(w, h);
    }
  }
  // somma pesata dei valori dei pixel vicini tramite una gaussiana
  return smooth_image(S, sigma);
}

// HW5 1.2
// Estimate the cornerness of each pixel given a structure matrix S.
// const Image& im S: structure matrix for an image.
// returns: a response map of cornerness calculations.
Image cornerness_response(const Image& S, int method){
  Image R(S.w, S.h);
  for (int w = 0; w < S.w; w++){
    for (int h = 0; h < S.h; h++){
      float IxIx = S(w, h, 0);
      float IyIy = S(w, h, 1);
      float IxIy = S(w, h, 2);
    
      R(w, h) = ((IxIx*IyIy) - pow(IxIy,2)) / (IxIx+IyIy); // Harris det(H) / trace(H)
    }
  }
  return R;
}


// HW5 1.3
// Perform non-max supression on an image of feature responses.
// const Image& im: 1-channel image of feature responses.
// int w: distance to look for larger responses.
// returns: Image with only local-maxima responses within w pixels.
Image nms_image(const Image& im, int w){
  //TIME(1);
  Image r=im;
  // perform NMS on the response map.
  // for every pixel in the image:
  //     for neighbors within w:
  //         if neighbor response greater than pixel response:
  //             set response to be very low
  
  for (int x = 0; x < im.w; x++)
    for (int y = 0; y < im.h; y++)
      for (int neighbor1 = x-w; neighbor1 <= x+w; neighbor1++)
        for (int neighbor2 = y-w; neighbor2 <= y+w; neighbor2++)
          if (im(x, y, 0) < im.clamped_pixel(neighbor1, neighbor2, 0))
            r(x, y, 0) = -1000;
  return r;
}


// HW5 1.4
// Perform corner detection and extract features from the corners.
// const Image& im: input image.
// const Image& nms: nms image
// float thresh: threshold for cornerness.
// returns: vector of descriptors of the corners in the image.
vector<Descriptor> detect_corners(const Image& im, const Image& nms, float thresh, int window){
  vector<Descriptor> d;
  for (int x = 0; x < im.w; x++)
    for (int y = 0; y < im.h; y++)
      if (nms(x, y, 0) > thresh) // se il pixel è un corner
        d.push_back(describe_index(im, x, y, window)); // aggiunge il descrittore del punto di interesse all'array di descrittori
  return d;
}

// Perform harris corner detection and extract features from the corners.
// const Image& im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
// returns: vector of descriptors of the corners in the image.
vector<Descriptor> harris_corner_detector(const Image& im, float sigma, float thresh, int window, int nms, int corner_method){
  // Calculate structure matrix
  Image S = structure_matrix(im, sigma);
  
  // Estimate cornerness
  Image R = cornerness_response(S,corner_method);
  
  // Run NMS on the responses
  Image Rnms = nms_image(R, nms);
  
  return detect_corners(im, Rnms, thresh, window);
}

// Find and draw corners on an image.
// Image& im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
Image detect_and_draw_corners(const Image& im, float sigma, float thresh, int window, int nms, int corner_method){
  vector<Descriptor> d = harris_corner_detector(im, sigma, thresh, window, nms, corner_method);
  return mark_corners(im, d);
}
