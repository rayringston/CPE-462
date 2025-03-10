#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
  string img_path;

  cout << "Please enter the path of the file you would like to edit: ";
  cin >> img_path;

  Mat image = imread(img_path, IMREAD_COLOR);
  imshow("Display Window", image);

  int k = waitkey(0);

}
