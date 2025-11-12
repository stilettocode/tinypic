
# tinypic

Energy matrix based image processing, using pixel-by-pixel calculations on images in PPM3 format where the pixels are exposed.

## Energy Matrices

The core idea behind the resizing and people-recognition algorithms in tinypic come from the idea of energy matrices. These are simply a matrix of number values that are the squared-difference between two pixels next to each other, specifically the euclidian difference between the RGB values. 

This creates a cool matrix that looks very similar to this (if you set it so that the highest energies are darker):

When you use the resizing tool in tinypic, it starts from the top of the matrix and finds the path to the bottom of the image using only the lowest energy pixels possible, or where the least information between pixels changes (in other words, the parts of the image where nothing is happening.) As the image is shrunk further and further, only more and more important pixels are left, important lines might be removed so it's important to keep track of exactly how small you're trying to resize your images. 

You can generally get away with this when you're dealing with non-human images (see the horse example later) but you really don't want someone's face to be warped and missing lines. So I added another algorithm that finds the pixel with the highest energy (usually found on the center of a person's body or face, due to facial features or outfit) and crops that out instead with no warping. 


## Demo

Remember to use PPM3 files for images!

People Crop (Non-warping):
```bash
./resize businessguy.ppm outputfile.ppm
```

```bash
./resize businesswoman.ppm outputfile.ppm
```

Content Crop (Warping)
```bash
./resize glorioushorses.ppm outputfile.ppm
```
Note the clouds changing in shape as their least important pixels are removed.
