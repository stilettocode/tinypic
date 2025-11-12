#ifndef PROCESSING_HPP
#define PROCESSING_HPP

#include "Matrix.hpp"
#include "Image.hpp"

// REQUIRES: img points to a valid Image
// MODIFIES: *img
// EFFECTS:  The image is rotated 90 degrees to the left (counterclockwise).
void rotate_left(Image* img);

// REQUIRES: img points to a valid Image.
// MODIFIES: *img
// EFFECTS:  The image is rotated 90 degrees to the right (clockwise).
void rotate_right(Image* img);

// REQUIRES: img points to a valid Image.
//           energy points to a Matrix.
// MODIFIES: *energy
// EFFECTS:  energy serves as an "output parameter".
//           The Matrix pointed to by energy is initialized to be the same
//           size as the given Image, and then the energy matrix for that
//           image is computed and written into it.
void compute_energy_matrix(const Image* img, Matrix* energy);

// REQUIRES: energy points to a valid Matrix.
//           cost points to a Matrix.
//           energy and cost aren't pointing to the same Matrix
// MODIFIES: *cost
// EFFECTS:  cost serves as an "output parameter".
//           The Matrix pointed to by cost is initialized to be the same
//           size as the given energy Matrix, and then the cost matrix is
//           computed and written into it.
void compute_vertical_cost_matrix(const Matrix* energy, Matrix *cost);

// REQUIRES: cost points to a valid Matrix
// EFFECTS:  Returns the vertical seam with the minimal cost according to the given
//           cost matrix, represented as a vector filled with the column numbers for
//           each pixel along the seam, starting with the lowest numbered row (top
//           of image) and progressing to the highest (bottom of image). The length
//           of the returned vector is equal to Matrix_height(cost).
//           While determining the seam, if any pixels tie for lowest cost, the
//           leftmost one (i.e. with the lowest column number) is used.
std::vector<int> find_minimal_vertical_seam(const Matrix* cost);

// REQUIRES: img points to a valid Image with width >= 2
//           seam.size() == Image_height(img)
//           each element x in seam satisfies 0 <= x < Image_width(img)
// MODIFIES: *img
// EFFECTS:  Removes the given vertical seam from the Image. That is, one
//           pixel will be removed from every row in the image. The pixel
//           removed from row r will be the one with column equal to seam[r].
//           The width of the image will be one less than before.
void remove_vertical_seam(Image *img, const std::vector<int> &seam);

// REQUIRES: img points to a valid Image
//           0 < newWidth && newWidth <= Image_width(img)
// MODIFIES: *img
// EFFECTS:  Reduces the width of the given Image to be newWidth by using
//           the seam carving algorithm.
void seam_carve_width(Image *img, int newWidth);

// REQUIRES: img points to a valid Image
//           0 < newHeight && newHeight <= Image_height(img)
// MODIFIES: *img
// EFFECTS:  Reduces the height of the given Image to be newHeight.
void seam_carve_height(Image *img, int newHeight);

// REQUIRES: img points to a valid Image
//           0 < newWidth && newWidth <= Image_width(img)
//           0 < newHeight && newHeight <= Image_height(img)
// MODIFIES: *img
// EFFECTS:  Reduces the width and height of the given Image to be newWidth
//           and newHeight, respectively.
void seam_carve(Image *img, int newWidth, int newHeight);

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
void crop_square_centered_at_max_energy(const Image* src, Image* dst);


#endif // PROCESSING_HPP
