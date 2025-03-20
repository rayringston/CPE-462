#include <opencv2/opencv.hpp>
#include <iostream>
#include <random>

using namespace std;
using namespace cv;

// Quality Improvement Functions

Mat gaussianBlur(Mat img) {
	// Uses a mask to create a blur
	Mat mask = (Mat_<double>(3, 3) << 1, 1, 1, 1, 1, 1, 1, 1, 1);
	mask = mask / 9;
	Mat temp = img.clone();
	filter2D(temp, temp, -1, mask);
	return temp;
}

Mat sharpen(Mat img) {
	Mat mask = (Mat_<double>(3, 3) << -1, -1, -1, -1, 9, -1, -1, -1, -1);
	Mat temp = img.clone();
	filter2D(temp, temp, -1, mask);
	return temp;
}

// Error Testing Functions

Mat saltPepper(Mat img, float intensity = 0.1, int type = 0) {
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

// Misc. Functions




int main() {
	int running = 1;

	Mat originalImg = imread("C:\\Users\\rayri\\Downloads\\apple.jpg", 0);
	Mat edited = originalImg;

	if (originalImg.empty()) {
		cout << "Image not found";
		return -1;
	}

	while (running == 1) {
		system("CLS");

		cout << "Please select which image technique you would like to use:" << endl;
		cout << "0. Exit" << endl;
		cout << "1. Gaussian Blur" << endl;
		cout << "2. Sharpen" << endl;
		cout << "3. Salt and Pepper Noise" << endl;

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
		
		switch (inVal) {
		case 0:
			running = 0;
			break;
		case 1:
			edited = gaussianBlur(edited);
			break;
		case 2:
			edited = sharpen(edited);
			break;
		case 3:
			int intensity; float type;
			string choice;

			cout << "Would you like to customize the salt & pepper noise? (Y/n): ";
			cin >> choice;
			if (choice == "y" or choice == "Y") {
				try {
					cout << "Please enter an intensity value from 0 to 1, no noise to only noise: ";
					cin >> choice;
					intensity = stof(choice);

					cout << "Please select a type of noise (1: salt, -1: pepper, other: both): ";
					cin >> choice;
					type = stoi(choice);

					edited = saltPepper(edited, intensity, type);
				}
				catch (const invalid_argument& error) {
					cout << "\nError: One of the arguments you entered was invalid, the process was cancelled.\n";
				}
			}
			else {
				edited = saltPepper(edited);
			}
			break;
		}
		Mat concatImg;
		hconcat(originalImg, edited, concatImg);

		imshow("Original | Edited", concatImg);

		waitKey(0);
	}

	return 0;
}

