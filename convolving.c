
/*************************************************************************
 * Homework: Image Convolution
 ************************************************************************/
#include <stdio.h>
#include <assert.h>
#include <MyroC.h>
#include <string.h>
#define HEIGHT 3
#define WIDTH 3


/* Zero out all pixels of a picture 
 Preconditions:  
   pic points to a valid Picture                          
 Postconditions:
   All pixels are zeroed to black:
     pic->pix_array[row][col].X = 0, for X one of R, G, or B, and
      0 <= row < 266 (from MyroC.h)    
      0 <= col < 427 (from MyroC.h)
 */
void         
zeroPic (Picture *pic) {
  memset( pic->pix_array, 0, sizeof(pic->pix_array) );
} // zero_pic 

/* Trims the inputed double val to 255 if its is larger than 255 and to 0 if its lesser than 0.
Preconditions: The value (val) inputed should be a double.
Postconditions: Returns the modified val as an unsigned char
 */
unsigned char
trim (double val)
{
  if (val < 0)
    val = 0;
  else if (val > 255)
    val = 255;

  return (unsigned char) val;
} // trim


/* Centers kernel at the Pixel located at the center of the inputed pic, 
and calculates the sum-product of the kernel with the brightnesses in 
each pixel color channel. 
Preconditions: The kernel should be entirely within image boundaries.
               0 <= pic->height < 266 (from MyroC.h)
               0 <= pic->width  < 427 (from MyroC.h)
Postconditions: Calls the function trim with an added bias and returns 
the modified pixel.
 */
Pixel
multiplyAdd (const Picture * pic, int center_row, int center_col,
             int kernel_height, int kernel_width,
             double kernel[][kernel_width], double bias)
{
  
  //initializes the pixel to a black pixel, preventing random values.
  Pixel pixel = {0, 0, 0}; 
  double green = 0; 
  double red = 0;
  double blue = 0;
  int row_current = center_row - (kernel_height/2); //keeps track of the position in the pic.
  int col_current = center_col - (kernel_width/2);
  assert(row_current >= 0);
  assert(col_current >= 0);
  assert((center_row + (kernel_height/2)) < (*pic).height);
  assert((center_col + (kernel_width/2)) < (*pic).width);
  //loops from negative half size to positive half size 
  for (int row = 0; row < kernel_height; row++, row_current++)
    {
      for (int col = 0; col < kernel_width; col++, col_current++)
        { 
          red += (*pic).pix_array[row_current][col_current].R
            *(kernel[row][col]);
          blue += (*pic).pix_array[row_current][col_current].B
            *kernel[row][col];
          green += (*pic).pix_array[row_current][col_current].G
            *kernel[row][col];
        }
      col_current = center_col - (kernel_width/2); //reset col as it goes to the next row
    }
  pixel.R = trim(red + bias); //call to trim with added bias
  pixel.G = trim(green + bias);
  pixel.B = trim(blue + bias);
 
  return pixel;
 
}//multiplyAdd


/*
Applies a kernel to all the pixels in the image by calling to multiplyAdd
Preconditions: pic type is a picture pointer, kernel_height and kernel_width are ints, kernel is an array of doubles and bias is a double.
               The kernell is within the boundaries of the image.
               0 <= pic->height < 266 (from MyroC.h)
               0 <= pic->width  < 427 (from MyroC.h)
Postconditions: Returns pix
 */
Picture
convolution (const Picture * pic, int kernel_height, int kernel_width,
             double kernel[][kernel_width], double bias)
{
  Picture modified_pic;
  //assigns the original width and height to the modified picture
  modified_pic.height = (*pic).height; 
  modified_pic.width = (*pic).width;
  //calls zeroPic to set the picture to black
  zeroPic(&modified_pic);
  int middle_col_k = kernel_width/2;
  int middle_row_k = kernel_height/2;
  int pic_row = (*pic).height;
  int pic_col = (*pic).width;
  //loops through every pixel in the image
  for(int row = middle_row_k; row < (pic_row - middle_row_k); row++)
    {
      for(int col = middle_col_k; col < (pic_col - middle_col_k); col++)
        {
          modified_pic.pix_array[row][col] = multiplyAdd(pic, row, col, kernel_height, kernel_width, kernel, bias);
        }
    }
  return modified_pic;

}//convolution

/* 
Bluring kernel in a 2D array, the height and width of the kernel 
are defined in the preprocessor. 
Smooths noise in an image.
*/
double bluring [HEIGHT][WIDTH] =
{
    { 0.0556 , 0.1246 , 0.0556 },
    { 0.1246 , 0.2791 , 0.1246 },
    { 0.0556 , 0.1246 , 0.0556 },
};//bluring

/* 
Spot_detection kernel in a 2D array, the height and width 
of the kernel are defined in the preprocessor. 
Laplacian operator, it is the second derivative of a 2D Gaussian.
*/
double spot_detection [HEIGHT][WIDTH] =
{
    { 0.25 , 0.50, 0.25},
    { 0.50 , -3.00, 0.50},
    { 0.25 , 0.50, 0.25 },
};//spot_detection

/* 
Emboss kernel in a 2D array, the height and width 
of the kernel are defined in the preprocessor. 
This filter is not included in the Homework page assignment.
Taken from: Image Kernels (detailed in references).
It converts every pixel to a higlight or a shadow, depending
on their original brightness.
*/
double emboss [HEIGHT][WIDTH] =
{
    { -2.0 , -1.0, 0.0 },
    { -1.0 , 1.0, 1.0 },
    { 0.0 , 1.0, 2.0 },
};//emboss


/* 
Sharpen kernel in a 2D array, the height and width 
of the kernel are defined in the preprocessor. 
Enhances the places that were mostly blured 
in the bluring filter.
*/
double sharpen [HEIGHT][WIDTH] =
{
    { 0.0 , -3.0, 0 },
    { -3.0 , 13.0, -3.0 },
    { 0.0 , -3.0, 0.0 },
};//sharpen

/* 
Edge Detect kernel in a 2D array, the height and width 
of the kernel are defined in the preprocessor. 
This filter is not included in the Homework page assignment.
Taken from: 8.2. Convolution Matrix (detailed in references)
It detects the edges of the images and highlights them.
*/
double edge_detect [HEIGHT][WIDTH] =
{
    { 0.0 , 1.0, 0.0 },
    { 1.0 , -4.0, 1.0 },
    { 0.0 , 1.0, 0.0 },
};//edge_detect


int
main (void)
{
  Picture pic = rLoadPicture("cookies.jpg");
  rDisplayPicture(&pic, 5, "Picture");
  //bluring
  Picture bluring_pic = convolution(&pic, HEIGHT, WIDTH, bluring, 0);
  rDisplayPicture(&bluring_pic, 10, "Filter picture");
  rSavePicture(&bluring_pic, "bluring_pic.jpg");
  //spot_detection
  Picture spot_pic = convolution(&pic, HEIGHT, WIDTH, spot_detection, 128);
  rDisplayPicture(&spot_pic, 10, "Filter picture");
  rSavePicture(&spot_pic, "spot_pic.jpg");
  //emboss
  Picture emboss_pic = convolution(&pic, HEIGHT, WIDTH, emboss, 0);
  rDisplayPicture(&emboss_pic, 10, "Filter picture");
  rSavePicture(&emboss_pic, "emboss_pic.jpg");
  //sharpen
  Picture sharpen_pic = convolution(&pic, HEIGHT, WIDTH, sharpen, 0);
  rDisplayPicture(&sharpen_pic, 10, "Filter picture");
  rSavePicture(&sharpen_pic, "sharpen_pic.jpg");
  //edge_detect
  Picture edge_pic = convolution(&pic, HEIGHT, WIDTH, edge_detect, 128);
  rDisplayPicture(&edge_pic, 10, "Filter picture");
  rSavePicture(&edge_pic, "edge_pic.jpg");
}//main
