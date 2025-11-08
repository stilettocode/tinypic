EECS 280 introduces the seam carving algorithm to CS students and challenges students with creating a content-aware image size reduction application. 
While completing the project, I realized that energy matrices don't just make the algorithm possible, but that they could be used for a practically
infinite number of image-editing reasons even for things that aren't content-aware image scaling. 

In this case, I noticed that in most business/professional photos, the area with the most energy ends up being the face due to a sharp contrast between the colors 
of certain facial features with others. With that in mind, I used the maximum energy values found within a matrix and cropped the area around it, 
producing a command-line application that does a pretty decent job at finding the face in most photos. I hope to incorporate additional editing techniques here
in the spring.

The application relies on PPM3 image files so that individual pixel data can be used for most calculations.
