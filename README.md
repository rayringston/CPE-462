# CPE-462 Final Project - Image Processing Toolbox

# Enhancement Capabilities
### Sharpen
This function simply uses a mask and convolutes it with the original image. Sharpen helps to remove any blurs within the image and to help define the edges. However, this technique is very sensitive to noise, so it recommended to first filter the image.

### Median Filtering
Median filtering functions similarly to the Guassian Blur, however, this function takes the median value of each 3x3 square in the image. Since it is the median and not the mean, the transformed value is not affected by the outliers, making this a use technique for removing salt and pepper noise.

### Gamma Correction
Cameras generally different responses to light compared to the human visual system. Certain shades of gray may be emphasized, causing images to become brighter or darker. Gamma correction can be used to readjust the scale of intensity in the image.

### Histogram Equalization
Histogram equalization is technique used to improve the contrast of the image. This is a transformation that uses the probablity of a given intensity to compute a more spread out distribution of intensities. Since the pixel intensities will be less concentrated, the image will appear to have better contrast. Additionally, there is a miscellaneous tool called Plot Histogram, that allows you to compare the histograms of the original and edited images.

### Contrast Stretching
Contrast stretching allows the user to create linear transformations over certain ranges of the image. 

# Noise Testing Tools

### Salt and Pepper Noise
In order to test many of the features in this toolbox, we also added the capabilities to add noise to an image. Salt and pepper noise appears as individual pixels becoming completely black or white. This is common in digital images, and is usually indicative of transmission errors or faulty memory.

This application of salt and pepper noise takes 2 parameters, intensity and type. Intensity is a float value from 0 to 1, 0 being no noise, and 1 being completely noise. Type is an integral value, 1 represents salt only, -1 is pepper, and anything else implements both salt and pepper.

### Gaussian Blur
The gaussian blur function smooths the image by applying a simple convolution. It does not take any parameters, and simply applies a normalized mask of all 1's to the original image. This has applications in removing noise from the image, and can also be used for aesthetic purposes. Also, since it removes some of the detail from the image, it can be used to test various other features, like sharpening

### Speckle Noise

# Miscellaneous Tools

### Thresholding
Thresholding is a technique used to help define certain fetaures in an image. Our implementation of includes global thresholding, an iterative global thresholf estimate algorithm, and adaptive thresholding. The global method allows the user to define a threshold, where any pixels dimmer will become black, and those brighter become white. The iterative algorithm applies a threshold to the image, and continuously calculates the average value of the pixels in the image. Then it reapplies a new threshold equal to this average value until the thresholding limit changes by less than 1%. The adaptive thresholding algorithm splits the original image into a rectangular grid of subimages. Each subimage is then run though the iterative estimage algorithm, selecting the ideal threshold for each subsection.

### Quantization

### Edge Detection

# Future Add
Exporting the edited image
