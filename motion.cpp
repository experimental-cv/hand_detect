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

  namedWindow("Grad");
  namedWindow("Contours");
  moveWindow("Grad", 200, 0);
  moveWindow("Contours", 900, 0);

  Mat prev;
  cap >> prev;
  Mat prev_gray;
  Mat now;
  Mat now_gray;
  Mat diff = Mat::zeros(prev.size(), CV_8UC1);
  Mat diff1 = Mat::zeros(prev.size(), CV_8UC1);
  Mat diff2 = Mat::zeros(prev.size(), CV_8UC1);
  Mat diff_combo = Mat::zeros(prev.size(), CV_8UC1);
  //Mat diff = Mat::zeros(prev.size(), CV_16FC1);

  for (;;) {
    cap >> now;
    if (now.empty()) // done
      return 0;

    cvtColor(now, now_gray, COLOR_BGR2GRAY);
    cvtColor(prev, prev_gray, COLOR_BGR2GRAY);

    //prev.convertTo(prev_gray, int rtype)

    absdiff(now_gray, prev_gray, diff); //now - prev;
    threshold(diff, diff, 5, 255, THRESH_BINARY);
    //cvtColor(diff, diff, COLOR_BGR2GRAY);
    //diff.convertTo(diff, COLOR_BGR2GRAY);
    now.copyTo(prev);
    //prev = now;

    // int erosion_type{MORPH_CROSS};
    // int erosion_size{1};
    // Mat element = getStructuringElement(
    //     erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1),
    //     Point(erosion_size, erosion_size));
    // erode(diff, diff, element);

    int operation{MORPH_OPEN};
    int morph_size{3};
    int morph_elem{MORPH_ELLIPSE};
    Mat element = getStructuringElement(
        morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1),
        Point(morph_size, morph_size));

    morphologyEx(diff, diff, operation, element);
    diff.copyTo(diff2);
    diff_combo = diff1 | diff2;
    diff2.copyTo(diff1);

    // NOTE: perhaps there is no real need to dilate
    // and lets dilate to merge the blobs!
    // int operation2{MORPH_OPEN};
    // int morph_size2{8};
    // int morph_elem2{MORPH_ELLIPSE};
    // Mat element2 = getStructuringElement(
    //     morph_elem2, Size(2 * morph_size2 + 1, 2 * morph_size2 + 1),
    //     Point(morph_size2, morph_size2));
    // dilate(diff_combo, diff_combo, element2);

    // find contours
    vector<vector<Point> > contours;
    findContours(diff_combo, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    // display bounding shapes of contours
    vector<vector<Point>> contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    vector<Point2f> centers(contours.size());
    vector<float> radius(contours.size());
    vector<RotatedRect> minRect(contours.size());

    for (size_t i = 0; i < contours.size(); i++) {
      approxPolyDP(contours[i], contours_poly[i], 3, true);
      boundRect[i] = boundingRect(contours_poly[i]);
      //minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
      minRect[i] = minAreaRect(contours[i]);
      //contours[i].
    }

    Mat drawing = now;
    //Mat drawing = Mat::zeros(grad.size(), CV_8UC3);

    RNG rng{0xFFFFFFFF};
    int detected_poi_count{0};
    Point2f avg_center{0.0f, 0.0f};

    float smallest_y = now.rows;
    int   smallest_i = -1;
    for (size_t i = 0; i < contours.size(); i++) {
      float area = minRect[i].size.height * minRect[i].size.width;
      float side_max = std::max(minRect[i].size.height, minRect[i].size.width);
      float side_min = std::min(minRect[i].size.height, minRect[i].size.width);
      float aspect = side_max / side_min;
      if (area < 30000 || aspect < 1.4) // select just the whole hand if possible
      //if (area < 3000) // cleans up all small stuff
      //if (area < 3000 || area > 50000)
        continue;
      cout << "Area: " << area << "\t Aspect: " << aspect << "\t Center: " << minRect[i].center << '\n';
      //minRect[i].center;
      avg_center += minRect[i].center;

      if (minRect[i].center.y < smallest_y) {
        smallest_y = minRect[i].center.y;
        smallest_i = i;
      }
      //minRect[i].size
      //if (boundRect[i].area() > 10000 || boundRect[i].area() < 100)
//        continue;

      Scalar color =
          Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
      drawContours(drawing, contours_poly, (int)i, color);
      //rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2);

      // min rectangle
      Point2f rect_points[4];
      minRect[i].points(rect_points);
      for (int j = 0; j < 4; j++) {
        line(drawing, rect_points[j], rect_points[(j + 1) % 4], color);
      }

      circle(drawing, centers[i], (int)radius[i], color, 2);
      //cout << "Rectangle area: " << boundRect[i].area() << '\n';
      ++detected_poi_count;
    }
    if (detected_poi_count > 0)
      cout << "Num poi: " << detected_poi_count << '\n';

    avg_center /= detected_poi_count;

    //cout << smallest_i << '\n';
    //circle(drawing, avg_center, 10, Scalar(0, 0, 255), 2);
    // if (smallest_i != -1) {
    //   //float avg_side = 0.5f * (minRect[smallest_i].size.height + minRect[smallest_i].size.width);
    //   //circle(drawing, minRect[smallest_i].center, 0.5f * avg_side, Scalar(0, 255, 0), 2);
    //   circle(drawing, minRect[smallest_i].center, 10, Scalar(0, 255, 0), 2);
    //   Point2f rect_points[4];
    //   minRect[smallest_i].points(rect_points);
    //   for (int j = 0; j < 4; j++) {
    //     line(drawing, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 0));
    //   }
    // }

    imshow("Grad", diff_combo);
    imshow("Contours", drawing);

    //imshow("grad", grad);
    //imshow("diff", diff);

    // 30 fps?
    if(waitKey(30) >= 0) break;
  }
  cap.release();
}
