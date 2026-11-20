
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

    cap >> frame;

    cv::imshow("Pick Measured Area", frame);
    cv::setMouseCallback("Pick Measured Area", LocationDetect, &frame);
    cv::waitKey(0);

    cv::destroyAllWindows();
    locationFile.close();

    std::ifstream locationInput("/Users/harrison/Documents/GitHub/CapstoneProject/dimensions.txt");

    std::string cord1, cord2, cord3, cord4;
    char comma = ',';

    std::getline(locationInput, cord1);

    size_t position = cord1.find(comma);

    std::string x1 = cord1.substr(1, position - 1);
    std::string y1 = cord1.substr(position + 2, cord1.length() - position - 3);


    std::getline(locationInput, cord2);

    position = cord2.find(comma);

    std::string x2 = cord2.substr(1, position - 1);
    std::string y2 = cord2.substr(position + 2, cord2.length() - position - 3);

    cv::Rect roi1(std::stoi(x1), std::stoi(y1), std::stoi(x2) - std::stoi(x1), std::stoi(y2) - std::stoi(y1));

    std::getline(locationInput, cord3);

    position = cord3.find(comma);

    std::string x3 = cord3.substr(1, position - 1);
    std::string y3 = cord3.substr(position + 2, cord3.length() - position - 3);

    std::getline(locationInput, cord4);

    position = cord4.find(comma);

    std::string x4 = cord4.substr(1, position - 1);
    std::string y4 = cord4.substr(position + 2, cord4.length() - position - 3);

    cv::Rect roi2(std::stoi(x3), std::stoi(y3), std::stoi(x4) - std::stoi(x3), std::stoi(y4) - std::stoi(y3));

    while (true) {
        cap >> frame;
        
        if (!cap.read(frame)) {
            std::cout << "Video ended or stream broken." << std::endl;
            break;
        }

        cv::split(frame, bgr_channels);

        cv::Mat blank = cv::Mat::zeros(frame.size(), CV_8UC1);
        cv::Mat green_colored;

        cv::merge(std::vector<cv::Mat>{blank, bgr_channels[1], blank}, green_colored);

        cv::Mat roi_img_1 = green_colored(roi1);

        cv::imshow("Green zone 1", roi_img_1);

        cv::Mat roi_img_2 = green_colored(roi2);

        cv::imshow("Green zone 2", roi_img_2);

        char key = (char)cv::waitKey(delay);
        if (key == 'q' || key == 27) {
            break;
        }
    }
    
    cap.release();
    cv::destroyAllWindows();
    locationInput.close();

    return 0;
}
