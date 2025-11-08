#include <cassert>
#include <vector>
#include "processing.hpp"

using namespace std;

// v DO NOT CHANGE v ------------------------------------------------
// The implementation of rotate_left is provided for you.
// REQUIRES: img points to a valid Image
// MODIFIES: *img
// EFFECTS:  The image is rotated 90 degrees to the left (counterclockwise).
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
// ^ DO NOT CHANGE ^ ------------------------------------------------

// v DO NOT CHANGE v ------------------------------------------------
// The implementation of rotate_right is provided for you.
// REQUIRES: img points to a valid Image.
// MODIFIES: *img
// EFFECTS:  The image is rotated 90 degrees to the right (clockwise).
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

  // Copy data back into original
  *img = aux;
}
// ^ DO NOT CHANGE ^ ------------------------------------------------


// v DO NOT CHANGE v ------------------------------------------------
// The implementation of diff2 is provided for you.
static int squared_difference(Pixel p1, Pixel p2) {
  int dr = p2.r - p1.r;
  int dg = p2.g - p1.g;
  int db = p2.b - p1.b;
  // Divide by 100 is to avoid possible overflows
  // later on in the algorithm.
  return (dr*dr + dg*dg + db*db) / 100;
}
// ^ DO NOT CHANGE ^ ------------------------------------------------


// ------------------------------------------------------------------
// You may change code below this line!



// REQUIRES: img points to a valid Image.
//           energy points to a Matrix.
// MODIFIES: *energy
// EFFECTS:  energy serves as an "output parameter".
//           The Matrix pointed to by energy is initialized to be the same
//           size as the given Image, and then the energy matrix for that
//           image is computed and written into it.
//           See the project spec for details on computing the energy matrix.
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
//           See the project spec for details on computing the cost matrix.
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
//           See the project spec for details on computing the minimal seam.
//           Note: When implementing the algorithm, compute the seam starting at the
//           bottom row and work your way up.
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
//           See the project spec for details on removing a vertical seam.
// NOTE:     Declare a new variable to hold the smaller Image, and
//           then do an assignment at the end to copy it back into the
//           original image.
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
//           the seam carving algorithm. See the spec for details.
// NOTE:     Use a vector to hold the seam, and make sure that it has
//           the right size. You can use .data() on a vector to get
//           the underlying array.
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
// NOTE:     This is equivalent to first rotating the Image 90 degrees left,
//           then applying seam_carve_width(img, newHeight), then rotating
//           90 degrees right.
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
// NOTE:     This is equivalent to applying seam_carve_width(img, newWidth)
//           and then applying seam_carve_height(img, newHeight).
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
//           The function aims to create a square of size 320x320, but
//           if that would run off the border while keeping the center
//           pixel fixed, the square is made smaller to the largest
//           possible odd-sized square centered on the pixel.
//           The returned square will always be at least 1x1 and will
//           not exceed the bounds of src.
void crop_square_centered_at_max_energy(const Image* src, Image* dst) {
  // compute energy matrix
  Matrix energy;
  compute_energy_matrix(src, &energy);

  // find max value
  int maxVal = Matrix_max(&energy);
  int maxR = 0;
  int maxC = 0;
  bool found = false;
  int h = Matrix_height(&energy);
  int w = Matrix_width(&energy);

  // Prefer an interior pixel: ignore the border which compute_energy_matrix
  // intentionally sets to the maximum. For tiny images (width<3 or height<3)
  // fall back to searching the whole image.
  if (w >= 3 && h >= 3) {
    for (int r = 1; r <= h - 2 && !found; ++r) {
      for (int c = 1; c <= w - 2; ++c) {
        if (*Matrix_at(&energy, r, c) == maxVal) {
          maxR = r;
          maxC = c;
          found = true;
          break;
        }
      }
    }
  }

  // fallback: search entire image if no interior max was found
  if (!found) {
    for (int r = 0; r < h && !found; ++r) {
      for (int c = 0; c < w; ++c) {
        if (*Matrix_at(&energy, r, c) == maxVal) {
          maxR = r;
          maxC = c;
          found = true;
          break;
        }
      }
    }
  }

  int srcW = Image_width(src);
  int srcH = Image_height(src);

  // available space around the center pixel
  int left_space = maxC;
  int right_space = srcW - 1 - maxC;
  int up_space = maxR;
  int down_space = srcH - 1 - maxR;

  int max_sym_radius = std::min(std::min(left_space, right_space), std::min(up_space, down_space));

  // desired target (aim for 320). We prefer an odd-sized square so the
  // pixel can be exactly centered. If 320 is even, use 319 which is
  // closest odd value.
  int desired = 320;
  if (desired % 2 == 0) desired -= 1; // 319
  // cannot exceed image dims
  desired = std::min(desired, std::min(srcW, srcH));

  // maximum odd-sized square we can center at the pixel
  int max_centered_size = 2 * max_sym_radius + 1;

  int side = std::min(desired, max_centered_size);
  if (side < 1) side = 1;

  int half = side / 2; // integer division; side is odd so this floors

  int top = maxR - half;
  int left = maxC - half;

  // initialize dst and copy pixels
  Image_init(dst, side, side);
  for (int r = 0; r < side; ++r) {
    for (int c = 0; c < side; ++c) {
      Image_set_pixel(dst, r, c, Image_get_pixel(src, top + r, left + c));
    }
  }
}
