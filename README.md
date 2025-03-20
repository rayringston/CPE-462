# CPE-462 Final Project - Image Processing Toolbox

# Improvement Capabilities
### Gaussian Blur
The gaussian blur function smooths the image by applying a simple convolution. It does not take any parameters, and simply applies a normalized mask of all 1's to the original image. This has applications in removing noise from the image, and can also be used for aesthetic purposes. Also, since it removes some of the detail from the image, it can be used to test various other features, like sharpening

### Sharpen
Similar to the blur, the sharpening function applies a mask to the image and convolves them. The resulting image has much more clearly defined features, and can help remove blurring from the image

### Salt and Pepper Noise
In order to test many of the features in this toolbox, we also added the capabilities to add noise to an image. Salt and pepper noise appears as individual pixels becoming completely black or white. This is common in digital images, and is usually indicative of transmission errors or faulty memory.

This application of salt and pepper noise takes 2 parameters, intensity and type. Intensity is a float value from 0 to 1, 0 being no noise, and 1 being completely noise. Type is an integral value, 1 represents salt only, -1 is pepper, and anything else implements both salt and pepper.

# Future Add
Contrast stretching, histogram equalization, gamma correction,
