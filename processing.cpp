#include <cassert>
#include <vector>
#include "processing.hpp"

//written by Ian Kim 

using namespace std;

void rotate_left(Image* img) {

  // for convenience
  int width = Image_width(img);
  int height = Image_height(img);

  // auxiliary image to temporarily store rotated image
  Image aux;
  Image_init(&aux, height, width); // width and height switched

  // iterate through pixels and place each where it goes in temp
  for (int r = 0; r < height; ++r) {
    for (int c = 0; c < width; ++c) {
      Image_set_pixel(&aux, width - 1 - c, r, Image_get_pixel(img, r, c));
    }
  }

  // Copy data back into original
  *img = aux;
}

void rotate_right(Image* img){

  // for convenience
  int width = Image_width(img);
  int height = Image_height(img);

  // auxiliary image to temporarily store rotated image
  Image aux;
  Image_init(&aux, height, width); // width and height switched

  // iterate through pixels and place each where it goes in temp
  for (int r = 0; r < height; ++r) {
    for (int c = 0; c < width; ++c) {
      Image_set_pixel(&aux, c, height - 1 - r, Image_get_pixel(img, r, c));
    }
  }

  *img = aux;
}

static int squared_difference(Pixel p1, Pixel p2) {
  int dr = p2.r - p1.r;
  int dg = p2.g - p1.g;
  int db = p2.b - p1.b;
  // Divide by 100 is to avoid possible overflows
  // later on in the algorithm.
  return (dr*dr + dg*dg + db*db) / 100;
}


// REQUIRES: img points to a valid Image.
//           energy points to a Matrix.
// MODIFIES: *energy
// EFFECTS:  energy serves as an "output parameter".
//           The Matrix pointed to by energy is initialized to be the same
//           size as the given Image, and then the energy matrix for that
//           image is computed and written into it.
void compute_energy_matrix(const Image* img, Matrix* energy) {
  Matrix_init(energy, Image_width(img), Image_height(img));

  for (int i = 1; i < Image_height(img) - 1; i++) {
    for (int j = 1; j < Image_width(img) - 1; j++) {
      *Matrix_at(energy, i, j) = squared_difference(Image_get_pixel(img, i - 1, j), Image_get_pixel(img, i + 1, j)) + 
      squared_difference(Image_get_pixel(img, i, j - 1), Image_get_pixel(img, i, j + 1));
    }
  }

  int curMax = Matrix_max(energy);
  Matrix_fill_border(energy, curMax);
}


// REQUIRES: energy points to a valid Matrix.
//           cost points to a Matrix.
//           energy and cost aren't pointing to the same Matrix
// MODIFIES: *cost
// EFFECTS:  cost serves as an "output parameter".
//           The Matrix pointed to by cost is initialized to be the same
//           size as the given energy Matrix, and then the cost matrix is
//           computed and written into it.
void compute_vertical_cost_matrix(const Matrix* energy, Matrix *cost) {
  Matrix_init(cost, Matrix_width(energy), Matrix_height(energy));

  for (int i = 0; i < energy->width; i++) {
    *Matrix_at(cost, 0, i) = *Matrix_at(energy, 0, i);
  }

  for (int i = 1; i < Matrix_height(energy); i++) {
    for (int j = 0; j < Matrix_width(energy); j++) {
      int left = std::max(0, j - 1);
      int right = std::min(Matrix_width(energy) - 1, j + 2);
  
      *Matrix_at(cost, i, j) = *Matrix_at(energy, i, j) + Matrix_min_value_in_row(cost, i - 1, left, right);
    }
  }
}


// REQUIRES: cost points to a valid Matrix
// EFFECTS:  Returns the vertical seam with the minimal cost according to the given
//           cost matrix, represented as a vector filled with the column numbers for
//           each pixel along the seam, with index 0 representing the lowest numbered
//           row (top of image). The length of the returned vector is equal to
//           Matrix_height(cost).
//           While determining the seam, if any pixels tie for lowest cost, the
//           leftmost one (i.e. with the lowest column number) is used.
vector<int> find_minimal_vertical_seam(const Matrix* cost) {
  vector<int> seamCalc(Matrix_height(cost));
  seamCalc[Matrix_height(cost) - 1] = Matrix_column_of_min_value_in_row(cost, Matrix_height(cost) - 1, 0, Matrix_width(cost));
  
  for (int i = Matrix_height(cost) - 2; i >= 0; i--) {
    int left = std::max(0, seamCalc[i + 1] - 1);
    int right = std::min(Matrix_width(cost) - 1, seamCalc[i + 1] + 1); //worst part of the project by far


    seamCalc[i] = Matrix_column_of_min_value_in_row(cost, i, left, right + 1);
  }

  return seamCalc;
}


// REQUIRES: img points to a valid Image with width >= 2
//           seam.size() == Image_height(img)
//           each element x in seam satisfies 0 <= x < Image_width(img)
// MODIFIES: *img
// EFFECTS:  Removes the given vertical seam from the Image. That is, one
//           pixel will be removed from every row in the image. The pixel
//           removed from row r will be the one with column equal to seam[r].
//           The width of the image will be one less than before.
void remove_vertical_seam(Image *img, const vector<int> &seam) {
  Image temp;
  Image_init(&temp, Image_width(img) - 1, Image_height(img));

  for (int i = 0; i < Image_height(img); i++) {
    for (int j = 0; j < seam[i]; j++) {
      Image_set_pixel(&temp, i, j, Image_get_pixel(img, i, j));
    }
    for (int j = seam[i] + 1; j < Image_width(img); j++) {
      Image_set_pixel(&temp, i, j - 1, Image_get_pixel(img, i , j));
    }
  }

  *img = temp;
}


// REQUIRES: img points to a valid Image
//           0 < newWidth && newWidth <= Image_width(img)
// MODIFIES: *img
// EFFECTS:  Reduces the width of the given Image to be newWidth by using
//           the seam carving algorithm.
void seam_carve_width(Image *img, int newWidth) {
  int runs = Image_width(img) - newWidth;

  Matrix opEnergy;
  Matrix opCost;
  vector<int> opSeam;

  for (int i = 0; i < runs; i++) {
    compute_energy_matrix(img, &opEnergy);
    compute_vertical_cost_matrix(&opEnergy, &opCost);
    opSeam = find_minimal_vertical_seam(&opCost);
    remove_vertical_seam(img, opSeam);
  }
}

// REQUIRES: img points to a valid Image
//           0 < newHeight && newHeight <= Image_height(img)
// MODIFIES: *img
// EFFECTS:  Reduces the height of the given Image to be newHeight.
void seam_carve_height(Image *img, int newHeight) {
  rotate_left(img);

  seam_carve_width(img, newHeight);

  rotate_right(img);
}

// REQUIRES: img points to a valid Image
//           0 < newWidth && newWidth <= Image_width(img)
//           0 < newHeight && newHeight <= Image_height(img)
// MODIFIES: *img
// EFFECTS:  Reduces the width and height of the given Image to be newWidth
//           and newHeight, respectively.
void seam_carve(Image *img, int newWidth, int newHeight) {
  seam_carve_width(img, newWidth);

  rotate_left(img);

  seam_carve_width(img, newHeight);

  rotate_right(img);
}


// REQUIRES: src points to a valid Image, dst points to an Image
// MODIFIES: *dst
// EFFECTS:  Finds the pixel in src with the highest energy value and
//           initializes *dst to be a square image centered on that pixel.
//           The function aims to create a square of size 512x512, but
//           if that would run off the border while keeping the center
//           pixel fixed, the square is made smaller to the largest
//           possible odd-sized square centered on the pixel.
//           The returned square will always be at least 1x1 and will
//           not exceed the bounds of src.
void crop_square_centered_at_max_energy(const Image* src, Image* dst) {
  // 1) compute energy
  Matrix energy;
  compute_energy_matrix(src, &energy);

  const int h = Matrix_height(&energy);
  const int w = Matrix_width(&energy);
  const int srcH = Image_height(src);
  const int srcW = Image_width(src);

  // 2) find max (prefer interior)
  int maxVal = Matrix_max(&energy);
  int maxR = 0, maxC = 0;
  bool found = false;

  if (w >= 3 && h >= 3) {
    for (int r = 1; r <= h - 2 && !found; ++r) {
      for (int c = 1; c <= w - 2; ++c) {
        if (*Matrix_at(&energy, r, c) == maxVal) {
          maxR = r; maxC = c; found = true; break;
        }
      }
    }
  }
  if (!found) {
    for (int r = 0; r < h && !found; ++r) {
      for (int c = 0; c < w; ++c) {
        if (*Matrix_at(&energy, r, c) == maxVal) {
          maxR = r; maxC = c; found = true; break;
        }
      }
    }
  }

  // guard against any mismatch between energy size and image size
  if (maxR < 0) maxR = 0;
  if (maxC < 0) maxC = 0;
  if (maxR >= srcH) maxR = srcH - 1;
  if (maxC >= srcW) maxC = srcW - 1;

  // clamp to image
  int target = 512;                     
  int side = std::min(target, std::min(srcW, srcH));

  // 4) center around (maxR, maxC) then clamp inside image
  int half = side / 2;                  
  int top  = maxR - half;
  int left = maxC - half;

  // clamp to keep the window fully inside the image
  if (top < 0) top = 0;
  if (left < 0) left = 0;
  if (top + side > srcH) top = srcH - side;
  if (left + side > srcW) left = srcW - side;

  // copy pixels
  Image_init(dst, side, side);
  for (int r = 0; r < side; ++r) {
    for (int c = 0; c < side; ++c) {
      Image_set_pixel(dst, r, c, Image_get_pixel(src, top + r, left + c));
    }
  }
}

