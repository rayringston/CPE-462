#include <opencv2/opencv.hpp>
#include <iostream>
#include <random>

using namespace std;
using namespace cv;

// ----------------- Quality Improvement Functions -------------------- //

Mat sharpen(Mat img) {
	Mat mask = (Mat_<double>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
	Mat temp = img.clone();
	filter2D(temp, temp, -1, mask);
	return temp;
}

Mat gammaCorrection(Mat img, float gamma = 1) {
	Mat temp = img.clone();
	
	for (int x = 0; x < temp.cols; x++) {
		for (int y = 0; y < temp.rows; y++) {
			float value = pow(((float) temp.at<uchar>(y, x) / 255), gamma) * 255;
			temp.at<uchar>(y, x) = value;
		}
	}

	return temp;
}

Mat medianFilter(Mat img, const int n = 3) {
	// Median filtering is very similar to the gaussian blur.
	// GB finds average value of the window, and MF finds the median
	Mat temp = img.clone();
	Mat bordered;
	// Without this line, there woudln't be enough values at the edges, so it would trim them off
	copyMakeBorder(temp, bordered, (n - 1) / 2, (n - 1) / 2, (n - 1) / 2, (n - 1) / 2, BORDER_REPLICATE);
	uchar* values = new uchar[n * n];

	for (int x = (n-1)/2; x <= bordered.cols - (n - 1) / 2 - 1; x++) {
		for (int y = (n - 1)/2; y <= bordered.rows - (n - 1) / 2 - 1; y++) {
			int pos = 0;

			for (int i = x - (n - 1) / 2; i <= x + (n - 1) / 2; i++) {
				for (int j = y - (n - 1) / 2; j <= y + (n - 1) / 2; j++) {
					values[pos] = bordered.at<uchar>(j, i);
					pos++;
				}
			}

			sort(values, values + sizeof(values)/sizeof(values[0]));

			temp.at<uchar>(y - (n-1)/2, x - (n-1)/2) = values[(int)floor((n * n) / 2)];
		}
	}

	return temp;
}

Mat contrastStretching(Mat img) {
	Mat temp = img.clone();

	// s = T(r)
	int r1, r2, s1, s2;

	cout << "Enter the first input gray level: ";
	cin >> r1;

	cout << "Enter the corresponding output level: ";
	cin >> s1;

	cout << "Enter the second input gray level: ";
	cin >> r2;

	cout << "Enter the corresponding output level: ";
	cin >> s2;
	
	float alpha = s1 / r2;
	float beta = (s2 - s1) / (r2 - r1);
	float gamma = (255 - s2) / (255 - r2);

	for (int x = 0; x < temp.cols; x++) {
		for (int y = 0; y < temp.rows; y++) {
			if (temp.at<uchar>(y, x) <= r1) {
				temp.at<uchar>(y, x) = alpha * temp.at<uchar>(y, x);
			}
			else if (temp.at<uchar>(y, x) <= r2) {
				temp.at<uchar>(y, x) = beta * (temp.at<uchar>(y, x) - r1) + s1;
			}
			else {
				temp.at<uchar>(y, x) = gamma * (temp.at<uchar>(y, x) - r2) + s2;
			}
		}
	}

	return temp;
}

// ----------------- Error Testing Functions -------------------- //

Mat saltPepper(Mat img, float intensity = 0.02, int type = 0) {
	// Simulates salt and pepper noise on an image
	// Intensity describes how noisy it is, higher is more noisy
	// Type : -1 is pepper, 1 is salt, 0 is both
	Mat temp = img.clone();

	for (int x = 0; x < temp.cols; x++) {
		for (int y = 0; y < temp.rows; y++) {
			float rv = (float)rand() / (float)RAND_MAX;
			if (rv < intensity) {
				if (type == -1) {
					temp.at<uchar>(y, x) = 0;
				}
				else if (type == 1) {
					temp.at<uchar>(y, x) = 255;
				}
				else {
					if (rand() < RAND_MAX / 2) {
						temp.at<uchar>(y, x) = 0;
					}
					else {
						temp.at<uchar>(y, x) = 255;
					}
				}
			}

		}
	}

	return temp;
}


Mat gaussianBlur(Mat img) {
	// Uses a mask to create a blur
	Mat mask = (Mat_<double>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1);
	mask = mask / 9;
	Mat temp = img.clone();
	filter2D(temp, temp, -1, mask);
	return temp;
}

Mat speckleNoise(Mat img, float sigma) {
	Mat temp = img.clone();

	// setup to generate the RVs for this type of noise
	random_device rd;
	mt19937 gen(rd());
	normal_distribution<float> dist{1, sigma };

	for (int x = 0; x < temp.cols; x++) {
		for (int y = 0; y < temp.rows; y++) {
			temp.at<uchar>(y, x) = round(temp.at<uchar>(y, x) * abs(dist(gen)));
			if (temp.at<uchar>(y, x) < 0) temp.at<uchar>(y, x) = 0;
			if (temp.at<uchar>(y, x) > 255) temp.at<uchar>(y, x) = 255;
		}
	}

	return temp;
}

// ----------------- Misc. Functions -------------------- //

Mat histogramEqualization(Mat img) {
	Mat temp = img.clone();
	float freqs[256] = { 0 };

	// Creates a list containing the frequency of each gray scale level
	for (int x = 0; x < temp.cols; x++) {
		for (int y = 0; y < temp.rows; y++) {
			freqs[temp.at<uchar>(y, x)] += 1;
		}
	}

	for (int i = 0; i < 256; i++) {
		freqs[i] = freqs[i] / (temp.cols * temp.rows);
	}

	for (int x = 0; x < temp.cols; x++) {
		for (int y = 0; y < temp.rows; y++) {
			float sum = 0;
			for (int i = 0; i < temp.at<uchar>(y, x); i++) {
				sum += freqs[i];
			}

			temp.at<uchar>(y, x) = round(255 * sum);
		}
	}

	return temp;
}

void checkEqualization(Mat img1, Mat img2) {
	float freqs1[256] = { 0 };
	float freqs2[256] = { 0 };
	for (int x = 0; x < img1.cols; x++) {
		for (int y = 0; y < img1.rows; y++) {
			freqs1[img1.at<uchar>(y, x)] += 1;
			freqs2[img2.at<uchar>(y, x)] += 1;
		}
	}
	
	cout << "Intensity | Freq (original) | Freq (edited)";
	for (int i = 0; i < 256; i++) {
		cout << i << " | " << freqs1[i] << " | " << freqs2[i] << endl;
	}
}

int main() {
	int running = 1;

	Mat originalImg = imread("C:\\Users\\rayri\\Downloads\\lenna.png", 0);
	Mat edited = originalImg;

	if (originalImg.empty()) {
		cout << "Image not found";
		return -1;
	}

	while (running == 1) {
		system("CLS");

		cout << "Please select which image technique you would like to use:" << endl;
		cout << "0. Exit" << endl;
		cout << "1. Sharpen" << endl;
		cout << "2. Median Filtering" << endl;
		cout << "3. Gamma Correction" << endl;
		cout << "4. Histogram Equalization" << endl;
		cout << "5. Contrast Stretching" << endl;
		cout << "6. Gaussian Blur" << endl;
		cout << "7. Salt and Pepper Noise" << endl;
		cout << "8. Speckle Noise" << endl;
		cout << "-1. Reset to Original" << endl;

		string input;
		cin >> input;

		int inVal;


		// checks if the number you entered was actually a number or not
		try {
			inVal = stoi(input);
		}
		catch (const invalid_argument& error) {
			cout << "That is not a valid selection.\n\n";
			continue;
		}
		
		string choice;

		switch (inVal) {
		case 0:
			running = 0;
			break;
		case 1:
			edited = sharpen(edited);
			break;
		case 2:
			edited = medianFilter(edited);
			break;
		case 3:
			cout << "Enter a decimal value for gamma: ";
			cin >> choice;

			edited = gammaCorrection(edited, stof(choice));
			break;
		case 4:
			edited = histogramEqualization(edited);
			checkEqualization(originalImg, edited);
			break;
		case 5:
			edited = contrastStretching(edited);
			break;
		case 6:
			edited = gaussianBlur(edited);
			break;
		case 7:
			float intensity; int type;

			cout << "Would you like to customize the salt & pepper noise? (Y/n): ";
			cin >> choice;
			if (choice == "y" or choice == "Y") {
				try {
					cout << "Please enter an intensity value from 0 to 1, no noise to only noise\n";
					cout << "This value is the probability a given pixel is noise, default is 0.02: ";
					cin >> choice;
					intensity = stof(choice);

					cout << "Please select a type of noise (1: salt, -1: pepper, other: both): ";
					cin >> choice;
					type = stoi(choice);

					edited = saltPepper(edited, intensity, type);
					cout << "check";
				}
				catch (const invalid_argument& error) {
					cout << "\nError: One of the arguments you entered was invalid, the process was cancelled.\n";
				}
			}
			else {
				edited = saltPepper(edited);
			}
			break;
		case 8:
			cout << "Please enter the variance of the normal distribution: ";
			cin >> choice;
			edited = speckleNoise(edited, stof(choice));
			break;

		case -1:
			edited = originalImg;
			break;
		}
		Mat concatImg;
		hconcat(originalImg, edited, concatImg);

		imshow("Original | Edited", concatImg);

		waitKey(0);
	}

	return 0;
}

