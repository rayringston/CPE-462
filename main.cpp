#include <opencv2/opencv.hpp>
#include <iostream>
#include <random>

// Found online, turns off the debug message in OpenCV
// For some reason these default to on
#include <opencv2/core/utils/logger.hpp>
cv::utils::logging::LogLevel level = cv::utils::logging::setLogLevel((cv::utils::logging::LogLevel) 0);

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

Mat contrastStretching(Mat img, int r1, int r2, int s1 ,int s2) {
	Mat temp = img.clone();

	// s = T(r)
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
			temp.at<uchar>(y, x) = (int) round(temp.at<uchar>(y, x) * abs(dist(gen)));
			if (temp.at<uchar>(y, x) < 0) temp.at<uchar>(y, x) = 0;
			if (temp.at<uchar>(y, x) > 255) temp.at<uchar>(y, x) = 255;
		}
	}

	return temp;
}

// ----------------- Misc. Functions -------------------- //

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

Mat thresholding(Mat img, int upperlimit=255, int lowerlimit=0) {
	Mat temp = img.clone();
	for (int x = 0; x < img.cols; x++) {
		for (int y = 0; y < img.rows; y++) {
			if (temp.at<uchar>(y, x) <= lowerlimit) {
				temp.at<uchar>(y, x) = 0;
			}
			if (temp.at<uchar>(y, x) >= upperlimit) {
				temp.at<uchar>(y, x) = 255;
			}
		}
	}
	return temp;
}

Mat quantization(Mat img, int bins) {
	Mat temp = img.clone();

	for (int x = 0; x < temp.cols; x++) {
		for (int y = 0; y < temp.rows; y++) {
			temp.at<uchar>(y, x) = (uchar) round((255.0 / (bins - 1)) * round((bins - 1) * temp.at<uchar>(y, x) / 255.0));
		}
	}

	return temp;
}

Mat edgeDetection(Mat img) {
	Mat sobelX = (Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	Mat sobelY = (Mat_<double>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);

	Mat temp = img.clone();
	Mat tempX, tempY, final = temp;

	filter2D(temp, tempX, -1, sobelX);
	filter2D(temp, tempY, -1, sobelY);

	double xSq, ySq, sum, finalVal;

	for (int x = 0; x < temp.cols; x++) {
		for (int y = 0; y < temp.rows; y++) {

			xSq = pow(((double)tempX.at<uchar>(y, x) / 255.0), 2);
			ySq = pow(((double)tempY.at<uchar>(y, x) / 255.0), 2);
			sum = xSq + ySq;
			finalVal = 255.0 * sqrt(sum);

			final.at<uchar>(y, x) = (uchar)round(finalVal);
		}
	}

	return final;
}

tuple<Mat, Mat> getImgFromFile(int quit, Mat oldImg, Mat oldEdited) {
	string filePath = "";
	Mat img;

	while (true) {
		if (quit == 1) cout << "Please enter the path to your desired image (Q to cancel): ";
		else cout << "Please enter the path to your desired image: ";
		
		cin >> filePath;

		if (quit == 1 and (filePath == "q" or filePath == "Q")) {
			return {oldImg, oldEdited};
		}

		// These lines are to make sure the file path is in the right format
		// OpenCV is really picky with that
		string tempPath;
		for (int i = 0; i < filePath.size(); i++) {
			if (filePath[i] == '/' and filePath[i + 1] != '/') {
				tempPath += "\\\\";
			}
			else if (filePath[i] == '\\' and filePath[i + 1] != '\\') {
				tempPath += "\\\\";
			}
			else if (filePath[i] == '\\' and filePath[i + 1] == '\\') {
			}
			else if (filePath[i] == '/' and filePath[i + 1] == '/') {
			}
			else {
				tempPath += filePath[i];
			}
		}
		filePath = tempPath;
		img = imread(filePath, 0);
		if (img.empty()) {
			
			cout << "Error: That is not a valid path, please write it from the root directory." << endl;
			cout << "Press ENTER to continue" << endl;

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cin.get();
			continue;
		}
		break;
	}

	return {img, img};
}

// ----------------- Main ------------------- //

int main(int argc, char* argv[]) {
	int running = 1;
	int displaying = 1;
	Mat originalImg, edited;

	if (argc >= 2) {
		originalImg = imread(argv[1], 0);
		edited = originalImg;
	}

	if (originalImg.empty()) {
		tie(originalImg,edited) = getImgFromFile(0, originalImg, originalImg);
	}

	//     C:\\Users\\rayri\\Downloads\\lenna.png"

	if (originalImg.empty()) {
		cout << "Image not found";
		return -1;
	}

	while (running == 1) {
		displaying = 1;
		system("CLS");

		cout << "Please select which image technique you would like to use:" << endl;
		cout << "0. Exit" << endl;
		cout << "<---------- Enhancement Tools" << endl;
		cout << "1. Sharpen" << endl;
		cout << "2. Median Filtering" << endl;
		cout << "3. Gamma Correction" << endl;
		cout << "4. Histogram Equalization" << endl;
		cout << "5. Contrast Stretching" << endl;
		cout << "<---------- Error Testing Tools" << endl;
		cout << "6. Gaussian Blur" << endl;
		cout << "7. Salt and Pepper Noise" << endl;
		cout << "8. Speckle Noise" << endl;
		cout << "<---------- Miscellaneous Tools" << endl;
		cout << "9. Thresholding" << endl;
		cout << "10. Quantization" << endl;
		cout << "11. Edge Detection" << endl;
		cout << "<---------- Settings" << endl;
		cout << "-1. Reset to Original" << endl;
		cout << "-2. Change Image" << endl;
		cout << "\n\n";

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
			try {
				cout << "Enter a decimal value for gamma: ";
				cin >> choice;
				edited = gammaCorrection(edited, stof(choice));
			}
			catch (const invalid_argument& error) {
				cout << "\nError: This value must be a decimal." << endl;
				cout << "Press ENTER to continue." << endl;

				// Got help with this online, after the bad read from the try, need to clear and ingore the flags in the cin
				// Then I wait for an enter with the cin.get
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cin.get();
				displaying = 0;
			}

			break;
		case 4:
			edited = histogramEqualization(edited);
			//checkEqualization(originalImg, edited);
			break;
		case 5:
			int r1, r2, s1, s2;
			try {
				cout << "Enter the first input gray level: ";
				cin >> choice;
				r1 = stoi(choice);

				cout << "Enter the corresponding output level: ";
				cin >> choice;
				s1 = stoi(choice);

				cout << "Enter the second input gray level: ";
				cin >> choice;
				r2= stoi(choice);

				cout << "Enter the corresponding output level: ";
				cin >> choice;
				s2 = stoi(choice);

				edited = contrastStretching(edited, r1, r2, s1, s2);
			}
			catch (const invalid_argument& error) {
				cout << "Error: This value must be an integer." << endl;
				cout << "Press ENTER to continue." << endl;

				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cin.get();
				displaying = 0;
			}
			
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
					cout << "Error: One of the arguments you entered was invalid, the process was cancelled." << endl;
					cout << "Press ENTER to continue." << endl;

					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cin.get();
					displaying = 0;
				}
			}
			else {
				edited = saltPepper(edited);
			}
			break;
		case 8:
			try {
				cout << "Please enter the variance of the normal distribution: ";
				cin >> choice;
				edited = speckleNoise(edited, stof(choice));
			}
			catch (const invalid_argument& error) {
				cout << "Error: This value must a demical." << endl;
				cout << "Press ENTER to continue." << endl;

				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cin.get();
				displaying = 0;
			}

			break;
		case 9:
			int lowerlimit, upperlimit;
			try {
				cout << "What is the value limit for the black threshold (0 has no effect): ";
				cin >> choice;
				lowerlimit = stoi(choice);

				cout << "What is the vaule limit for the white threshold (255 has no effect): ";
				cin >> choice;
				upperlimit = stoi(choice);

				edited = thresholding(edited, upperlimit, lowerlimit);
			}
			catch (const invalid_argument& error) {
				cout << "Error: This value must an integer." << endl;
				cout << "Press ENTER to continue." << endl;

				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cin.get();
				displaying = 0;
			}

			break;
		case 10:
			int bins;
			try {
				cout << "How many distinct levels would you like (standard is 255): ";
				cin >> choice;
				bins = stoi(choice);
				edited = quantization(edited, bins);
			}
			catch (const invalid_argument& error) {
				cout << "Error: This value must an integer." << endl;
				cout << "Press ENTER to continue." << endl;

				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cin.get();
				displaying = 0;
			}
			break;
		case 11:
			edited = edgeDetection(edited);
			break;
		case -1:
			edited = originalImg;
			break;
		case -2:
			tie(originalImg, edited) = getImgFromFile(1, originalImg, edited);
			break;
		}

		Mat concatImg;
		hconcat(originalImg, edited, concatImg);

		if (displaying == 1) {
			imshow("Original | Edited", concatImg);
		}

		waitKey(0);
	}

	string choice;
	cout << "Would you like to save this edited image (Y/n): ";
	cin >> choice;
	/*
	if (choice == "Y") {
		cout << "Please write the full path of your destination." << endl;
		cout << "It will be saved as a .jpg file, so do not include the extension: ";
		cin >> choice;

		imwrite(choice + ".jpg", edited);
	}
	*/
	cout << "<-------------------------->" << endl;
	cout << "Ray Ringstong & Ardit Cana" << endl;
	cout << "<-------------------------->" << endl;

	return 0;
}

