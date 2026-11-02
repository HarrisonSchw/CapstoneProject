
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


void LocationDetect(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        std::ofstream locationFile("/Users/harrison/Documents/GitHub/CapstoneProject/dimensions.txt", std::ios::app);
        locationFile << "(" << x << ", " << y << ")" << std::endl;
        locationFile.close();
    }
}


int main() {
    std::ofstream locationFile("/Users/harrison/Documents/GitHub/CapstoneProject/dimensions.txt");

     
        if (!locationFile.is_open()) {
            std::cerr << "Error: Could not open the file!" << std::endl;
            return 1;
        }

    cv::VideoCapture cap("/Users/harrison/Documents/GitHub/CapstoneProject/ACSTest.mov", cv::CAP_AVFOUNDATION);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file or camera feed." << std::endl;
        return -1;
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    int delay = (fps > 0) ? (1000 / fps) : 30;

    cv::Mat frame;
    cv::namedWindow("Video Playback", cv::WINDOW_AUTOSIZE);
    std::vector<cv::Mat> bgr_channels;

    int frame_count = 0;
    // 3. Continuous processing loop
    while (true) {

        cap >> frame;
        // Read the next frame. Returns false if the video ends or disconnects.
        if (!cap.read(frame)) {
            std::cout << "Video ended or stream broken." << std::endl;
            break;
        }

        cv::split(frame, bgr_channels);

        cv::Mat blank = cv::Mat::zeros(frame.size(), CV_8UC1);
        cv::Mat green_colored;

        cv::merge(std::vector<cv::Mat>{blank, bgr_channels[1], blank}, green_colored);

        cv::imshow("Isolated Green", green_colored);
        if (frame_count == 0) {
            cv::setMouseCallback("Isolated Green", LocationDetect, &green_colored);
            cv::waitKey(0);
        }
        

        char key = (char)cv::waitKey(delay);
        if (key == 'q' || key == 27) {
            break;
        }
        frame_count++;
    }
    
    cap.release();
    cv::destroyAllWindows();
    locationFile.close();

    return 0;
}
