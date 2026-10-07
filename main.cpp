
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>




int main() {
    
    std::cout << "Hello, CMake World!" << std::endl;

    cv::VideoCapture cap("testvid.mov");
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file or camera feed." << std::endl;
        return -1;
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    int delay = (fps > 0) ? (1000 / fps) : 30; // fallback to ~30ms delay if FPS is unavailable

    cv::Mat frame;
    cv::namedWindow("Video Playback", cv::WINDOW_AUTOSIZE);

    // 3. Continuous processing loop
    while (true) {
        // Read the next frame. Returns false if the video ends or disconnects.
        if (!cap.read(frame)) {
            std::cout << "Video ended or stream broken." << std::endl;
            break;
        }

        // Display the frame in the window
        cv::imshow("Video Playback", frame);

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
