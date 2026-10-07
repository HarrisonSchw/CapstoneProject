
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>




int main() {
    

    cv::VideoCapture cap(0, cv::CAP_AVFOUNDATION);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file or camera feed." << std::endl;
        return -1;
    }

    cv::Mat frame;
    cv::namedWindow("Video Playback", cv::WINDOW_AUTOSIZE);
    std::vector<cv::Mat> bgr_channels;

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

        // 4. Wait for key press and check if 'q' or 'Esc' (27) was pressed to exit
        char key = (char)cv::waitKey(delay);
        if (key == 'q' || key == 27) {
            break;
        }
    }

    // 5. Clean up (Optional, but best practice)
    cap.release();
    cv::destroyAllWindows();


    return 0;
}
