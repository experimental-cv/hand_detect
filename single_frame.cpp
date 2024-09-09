#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>

using namespace std;
using namespace cv;
int main(int argc, const char *argv[]) {
  //std::cout << cv::getBuildInformation() << std::endl;
  //std::cout << "Welcome to OpenCV " << CV_VERSION << std::endl;

  // command line parser
  const char* keys =
    "{help h usage ? | | print this message}"
    "{@video | | Video file. If not specified, try to use webcamera}";

  CommandLineParser parser(argc, argv, keys);

  if (parser.has("help")) {
    parser.printMessage();
    return 0;
  }

  String videoFile{parser.get<String>(0)};
  if (!parser.check()) {
    parser.printErrors();
    return 0;
  }

  VideoCapture cap;
  if (videoFile != "")
    cap.open(videoFile);
  else
    cap.open(0);
  if (!cap.isOpened())
    return -1;

  Mat prev;
  Mat now;
  Mat mat_gray;
  Mat mat_blur;
  Mat mat_canny1;
  Mat mat_canny2;
  Mat mat_canny3;
  Mat mat_canny4;
  Mat mat_canny;

  int erosion_type{MORPH_CROSS};
  int erosion_size{1};
  Mat erosion_element = getStructuringElement(
    erosion_type,
    Size(2*erosion_size + 1, 2*erosion_size+1),
    Point(erosion_size, erosion_size));
  Mat mat_erosion;
  //Mat mat_canny2;

  Mat mat_lines;
  // detect lines
  vector<Vec4i> lines;
  vector<Vec3i> circles; // circle position + density

  // namedWindow("Gray");
  namedWindow("Blur");
  namedWindow("Canny");
  // moveWindow("Gray", 0, 0);
  moveWindow("Blur", 0, 0);
  moveWindow("Canny", 800, 0);
  moveWindow("Lines", 0, 800);
  cap >> prev;

  // initialize mat_canny1 and mat_canny2
  mat_canny1 = Mat::zeros(prev.size(), CV_8UC1);
  mat_canny2 = Mat::zeros(prev.size(), CV_8UC1);
  mat_canny3 = Mat::zeros(prev.size(), CV_8UC1);
  mat_canny4 = Mat::zeros(prev.size(), CV_8UC1);
  mat_canny = Mat::zeros(prev.size(), CV_8UC1);
  for (;;) {
    cap >> now;
    if (now.empty()) // done
      return 0;
    //float alpha{0.01};
    //now = now*alpha + prev*(1-alpha);
    //addWeighted(now, alpha, prev, 1-alpha, 0.0, now);
    bilateralFilter(now, mat_gray, 3, 6, 1);
    cvtColor(mat_gray, mat_gray, COLOR_BGR2GRAY);
    //cvtColor(now, mat_gray, COLOR_BGR2GRAY);
    mat_blur = mat_gray;
    //bilateralFilter(mat_gray, mat_blur, 3, 6, 1);
    //blur(mat_gray, mat_blur, Size(3, 3));

    int lowThreshold{20};
    int ratio{2};
    int kernel_size{3};
    Canny(mat_blur, mat_canny2, lowThreshold, lowThreshold * ratio, kernel_size);
    //Mat mat_canny_and1 = mat_canny1 | mat_canny2;
    //Mat mat_canny_and2 = mat_canny3 | mat_canny3;
    //dilate(mat_canny_and1, mat_canny_and1, erosion_element);
    //dilate(mat_canny_and2, mat_canny_and2, erosion_element);

    //mat_canny = mat_canny_and1 & mat_canny_and2;
    //mat_canny = (mat_canny1 | mat_canny2) & (mat_canny3 | mat_canny4);
    //mat_canny = (mat_canny1 | mat_canny2) & (mat_canny3 | mat_canny4);
    mat_canny = mat_canny1 | mat_canny2;
    mat_canny2.copyTo(mat_canny1);

    Mat mat_dist;
    distanceTransform(mat_canny, mat_dist, DIST_L2, 3);
    normalize(mat_dist, mat_dist, 0, 1.0, NORM_MINMAX);
    //mat_canny3.copyTo(mat_canny2);
    //mat_canny4.copyTo(mat_canny3);

    //dilate(mat_canny, mat_erosion, erosion_element);

    // int lowThreshold2{40};
    // int ratio2{3};
    // int kernel_size2{3};
    // Canny(mat_erosion, mat_canny2, lowThreshold2, lowThreshold2 * ratio2, kernel_size2);

    // dilate(mat_canny2, mat_erosion, erosion_element);
    // erode(mat_erosion, mat_erosion, erosion_element);

    // lines.clear();
    // HoughLinesP(mat_canny, lines, 2, CV_PI/90, 20, 20, 5 );
    // //HoughLines(mat_canny, lines, 16, CV_PI/90, 150, 0, 0 ); // detect lines
    // // draw lines
    // mat_lines = Mat::zeros(now.size(), now.type());
    // for( size_t i{0}; i < lines.size(); ++i) {
    //   Vec4i l = lines[i];
    //   //circle(mat_lines, Point(l[0], l[1]), 2, Scalar(0, 0, 255), FILLED, LINE_8);
    //   //circle(mat_lines, Point(l[2], l[3]), 2, Scalar(0, 0, 255), FILLED, LINE_8);
    //   //mat_lines.at
    //   //line(mat_lines, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 2, LINE_AA);
    //   line(mat_lines, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 2, LINE_AA);

    //   // - treating both endpoints of a line as a circle,
    //   //   now we want to remove circles that are surrounded on all sides by
    //   //   other circles that are located too close to the center circle
    //   // - but we don't want to remove circles that are surrounded just on one
    //   // side
    //   // - we can start by computing density of a circle, and plotting it as a
    //   // color
    //   //

    //   for (size_t j{i+1}; j < lines.size(); ++j) {

    //   }
    // }
    //cout << "Num lines: " << lines.size() << '\n';
    if (!now.empty() && !prev.empty()) {
      //Rect rect_roi(0,frame.rows-101,frame.cols-1,100);
      //Mat img_roi{frame(rect_roi)};
      //imshow("Video", img_roi);
      //imshow("Video", frame);
      //imshow("Combo", prev.mul(now));
      //imshow("Combo", now - prev);
      //Canny()

      //imshow("Original", now);
      //imshow("Gray", mat_gray);
      imshow("Blur", mat_blur);
      imshow("Canny", mat_canny);
      imshow("Lines", mat_dist);
      //imshow("Canny2", mat_canny2);
      //imshow("Erosion2", mat_erosion);
      //imshow("Lines", mat_lines);
      //imshow("Now+Lines", now);
      prev = now;
    }

    if(waitKey(10) >= 0) break;
  }
  cap.release();
}
