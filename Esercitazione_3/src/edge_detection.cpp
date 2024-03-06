#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"

#define M_PI 3.14159265358979323846


/*
Smooths a grayscale image by convolving it with a Gaussian kernel of standard deviation sigma.
Input:
    Image im: the input image
    float sigma: the standard deviation of the Gaussian kernel
Output:
    Image: the smoothed image (im.w, im.h, 1)
*/
Image smooth_image(const Image& im, float sigma) {
    Image gauss = make_gaussian_filter(sigma);
    Image ret = convolve_image(im, gauss, false);
    return ret;
}

/*
Computes the magnitude and direction of the gradient of an image.
Input:
    Image im: the input image
Output:
    pair<Image,Image>: the magnitude and direction of the gradient of the image
                       with magnitude in [0,1] and direction in [-pi,pi]
*/
pair<Image,Image> compute_gradient(const Image& im) {
    pair<Image, Image> sobel = sobel_image(im);
    feature_normalize(sobel.first);
    return sobel;
}


/*
Performs non-maximum suppression on an image.
Input:
    Image mag: the magnitude of the gradient of the image [0,1]
    Image dir: the direction of the gradient of the image [-pi,pi]
Output:
    Image: the image after non-maximum suppression
*/
float min_mag (float a, float b, float c){
    return min({a,b,c});
}

Image non_maximum_suppression(const Image& mag, const Image& dir) {
    Image nms(mag.w, mag.h, 1);
    float neighbor1, neighbor2;

    // Iterate through the image and perform non-maximum suppression
    for (int y = 0; y < mag.h; y++) {
        for (int x = 0; x < mag.w; x++) {
            // Get the direction of the gradient at the current pixel
            float curr_dir_f = (float)(dir(x,y,0)/M_PI)*180;
            int curr_dir = round(curr_dir_f);
            if (curr_dir < 0) curr_dir += 180;
            int near_dir;

            // Round the direction to the nearest multiple of PI/4
            int dist = curr_dir % 45;
            if (dist > 22) near_dir = curr_dir - dist + 45;
            else near_dir = curr_dir - dist;
            
            // Get the magnitude of the gradient of the two neighbors along that direction
            // (Hint: use clamped_pixel to avoid going out of bounds)
            if (near_dir == 0 || near_dir == 180) {
                neighbor1 = mag.clamped_pixel(x-1,y,0);
                neighbor2 = mag.clamped_pixel(x+1,y,0);
            }
            else if (near_dir == 90) {
                neighbor1 = mag.clamped_pixel(x,y+1,0);
                neighbor2 = mag.clamped_pixel(x,y-1,0);
            }
            else if (near_dir == 135) {
                neighbor1 = mag.clamped_pixel(x+1,y-1,0);
                neighbor2 = mag.clamped_pixel(x-1,y+1,0);
            }
            else if (near_dir == 45) {
                neighbor1 = mag.clamped_pixel(x-1,y-1,0);
                neighbor2 = mag.clamped_pixel(x+1,y+1,0);
            }

            // If the magnitude of the gradient of the current pixel is greater than that of both neighbors,
            // then it is a local maximum
            if (mag(x,y,0) >= neighbor1 && mag(x,y,0) >= neighbor2) nms.set_pixel(x,y,0,mag(x,y,0));
            else nms.set_pixel(x,y,0,0);
        }
    }
    return nms;
}



/*
    Applies double thresholding to an image.
    Input:
        Image im: the input image
        float lowThreshold: the low threshold value
        float highThreshold: the high threshold value
        float strongVal: the value to use for strong edges
        float weakVal: the value to use for weak edges
    Output:
        Image: the thresholded image
*/
Image double_thresholding(const Image& im, float lowThreshold, float highThreshold, float strongVal, float weakVal)
{
    Image res(im.w, im.h, im.c);
    
    for (int c=0; c<im.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                float curr_pix = im(x,y,c);
                if (curr_pix > highThreshold) res.set_pixel(x,y,c,strongVal);
                else if(curr_pix < lowThreshold) res.set_pixel(x,y,c,0);
                else res.set_pixel(x,y,c,weakVal);
            }
        }
    }

    return res;
}


/*
    Applies hysteresis thresholding to an image.
    Input:
        Image im: the input image
        float weak: the value of the weak edges
        float strong: the value of the strong edges
    Output:
        Image: the image after hysteresis thresholding, with only strong edges
*/
Image edge_tracking(const Image& im, float weak, float strong)
{
    Image res(im.w, im.h, im.c);

    for (int c=0; c<im.c; c++) {
        for (int y=0; y < im.h; ++y) {
            for (int x=0; x < im.w; ++x) {
                if (im(x,y,c) == strong) { res.set_pixel(x,y,c,strong); continue; }
                if (im(x,y,c) == 0) { res.set_pixel(x,y,c,0); continue; }
                
                int set = 0;
                for (int i=-1; i<2; i++) {
                    for (int j=-1; j<2; j++) {
                        
                        if(im.clamped_pixel(x+i,y+j,c) == strong) {
                            res.set_pixel(x,y,c,strong); 
                            set = 1; 
                            break;
                        }   
                    }
                }
                if (set == 0) res.set_pixel(x,y,c,0);
            }
        }
    }
    return res;
}
