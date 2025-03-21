# CPE-462 Final Project - Image Processing Toolbox

# Enhancement Capabilities
### Sharpen
This function simply uses a mask and convolutes it with the original image. Sharpen helps to remove any blurs within the image and to help define the edges.

### Median Filtering
Median filtering functions similarly to the Guassian Blur, however, this function takes the median value of each 3x3 square in the image. Since it is the median and not the mean, the transformed value is not affected by the outliers, making this a use technique for removing salt and pepper noise.

### Gamma Correction
Cameras generally different responses to light compared to the human visual system. Certain shades of gray may be emphasized, causing images to become brighter or darker. Gamma correction can be used to readjust the scale of intensity in the image.

### Histogram Equalization
### Contrast Stretching

# Noise Testing Tools

### Salt and Pepper Noise
In order to test many of the features in this toolbox, we also added the capabilities to add noise to an image. Salt and pepper noise appears as individual pixels becoming completely black or white. This is common in digital images, and is usually indicative of transmission errors or faulty memory.

This application of salt and pepper noise takes 2 parameters, intensity and type. Intensity is a float value from 0 to 1, 0 being no noise, and 1 being completely noise. Type is an integral value, 1 represents salt only, -1 is pepper, and anything else implements both salt and pepper.

### Gaussian Blur
The gaussian blur function smooths the image by applying a simple convolution. It does not take any parameters, and simply applies a normalized mask of all 1's to the original image. This has applications in removing noise from the image, and can also be used for aesthetic purposes. Also, since it removes some of the detail from the image, it can be used to test various other features, like sharpening

### Speckle Noise

# Future Add
Not sure
