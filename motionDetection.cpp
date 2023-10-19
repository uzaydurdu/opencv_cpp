#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "motionDetection.h"

using namespace cv;
using namespace std;

float colorDistance(cv::Vec3b color1, cv::Vec3b color2)
{
    double dr = color2[2] - color1[2];
    double dg = color2[1] - color1[1];
    double db = color2[0] - color1[0];
    return (dr * dr + dg * dg + db * db);
}

int main() {
    cv::VideoCapture cap(0);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open the camera." << std::endl;
        return -1;
    }

    cv::Mat frame;
    cv::Mat previousFrame;

    cap >> previousFrame;

    while (true) {
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "Error: Frame is empty." << std::endl;
            break;
        }

        cv::namedWindow("Camera Feed");

        float motX = 0.0f, motY = 0.0f, lerpX = 0.0f, lerpY = 0.0f, avgX = 0.0f, avgY = 0.0f, threshold = 50.0f;
        int count = 0;

        for (int x = 0; x < frame.cols; x++) {
            for (int y = 0; y < frame.rows; y++) {
                cv::Vec3b curCol = frame.at<cv::Vec3b>(y, x);
                cv::Vec3b prevCol = previousFrame.at<cv::Vec3b>(y, x);
                float dist = colorDistance(curCol, prevCol);

                if (dist > threshold * threshold) {
                    avgX += x;
                    avgY += y;
                    count++;
                    frame.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);  // Set the pixel to white for motion
                }
                else{
                    frame.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
                }
            }
        }

        if (count > 200) {
            motX = avgX / count;
            motY = avgY / count;
        }

        lerpX = 0.1 * motX + (1.0 - 0.1) * lerpX;
        lerpY = 0.1 * motY + (1.0 - 0.1) * lerpY;

        cv::circle(frame, cv::Point(static_cast<int>(motX), static_cast<int>(motY)), 24, cv::Vec3b(128, 0, 44), -1);

        cv::imshow("Camera Feed", frame);
        cap >> previousFrame; // Update the previous frame
        char key = cv::waitKey(10);
        if (key == 27) {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}
