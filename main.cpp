#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

cv::Vec3b targetColor(0, 0, 255); 
cv::Point clickedPoint(-1, -1);

// Calculate the Euclidean distance between two colors
float colorDistance(cv::Vec3b color1, cv::Vec3b color2)
{
    double dr = color2[2] - color1[2];
    double dg = color2[1] - color1[1];
    double db = color2[0] - color1[0];
    return (dr * dr + dg * dg + db * db);
}

void onMouse(int event, int x, int y, int flags, void *userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        cv::Mat *image = static_cast<cv::Mat *>(userdata);
        cv::Vec3b color = image->at<cv::Vec3b>(y, x);

        targetColor = color;
        int blue = color[0];
        int green = color[1];
        int red = color[2];

        std::cout << "Pixel color at (" << x << ", " << y << "): B = " << blue << ", G = " << green << ", R = " << red << std::endl;
    }
}

int main()
{
    cv::VideoCapture cap(0);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open the camera." << std::endl;
        return -1;
    }

    while (true)
    {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty())
        {
            std::cerr << "Error: Frame is empty." << std::endl;
            break;
        }

        cv::namedWindow("Camera Feed");
        cv::setMouseCallback("Camera Feed", onMouse, &frame);

        cv::circle(frame, clickedPoint, 20, targetColor, -1);

        float avgX = 0, avgY = 0, threshold = 30.0f;
        int count = 0;

        for (int x = 0; x < frame.cols; x++)
        {
            for (int y = 0; y < frame.rows; y++)
            {
                cv::Vec3b curCol = frame.at<cv::Vec3b>(y, x);
                float dist = colorDistance(curCol, targetColor);

                if (dist < threshold*threshold)
                { 
                    avgX += x;
                    avgY += y;
                    count++;
                }
            }
        }

        if (count > 0)
        {
            avgX = avgX / count;
            avgY = avgY / count;
            cv::circle(frame, cv::Point(avgX, avgY), 24, targetColor, 2);
        }

        cv::imshow("Camera Feed", frame);

        char key = cv::waitKey(10);
        if (key == 27)
        {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
