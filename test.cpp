
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>


// Change ROI Code to be sized on robot


int fluorescence(const std::string& imagePath, std::vector<double>& wormFluorescence);

int main() {
    std::vector<double> wormFluorescence;
    std::vector<std::string> imNames;

    std::filesystem::path folder = "/Users/harrison/Documents/HelloWorld/Data";

    if (!std::filesystem::exists(folder) || !std::filesystem::is_directory(folder)) {
        std::cout << "Invalid folder\n";
        return 1;
    }

    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".bmp" || ext == ".jpg" || ext == ".png") {
                fluorescence("/Users/harrison/Documents/HelloWorld/Data/" + entry.path().filename().string(), wormFluorescence);
                imNames.push_back(entry.path().filename().string());
            }

        }
    }

    return 0;
}


int fluorescence(const std::string& imagePath,  std::vector<double>& wormFluorescence) {
    const int numWorms = 1; // CHANGE TO BE THE NUMBER OF WORMS THAT ARE BEING SEARCHED FOR
    const int roiSize = 600; // CHANGE TO BE ~ 2x # PIXELS OF LENGTH OF FLUORESCENT AREA
    const int maxSearch = 5; // CHANGE TO BE THE MAXIMUM NUMBER OF TIMES YOU WANT TO SEARCH
    
    cv::Mat gray = cv::imread(imagePath, cv::IMREAD_GRAYSCALE); // Read the image in grayscale
    std::vector<cv::Rect> checkedWorms; // Initialize vector of areas checked for worms

    // Check if the image was successfully loaded
    if (gray.empty()) {
        std::cout << "Could not open or find the image!" << std::endl;
        return -1;
    }

    // INTERPOLATION FOR DEAD PIXELS CHANGE TO MATCH CAMERA OR DELETE
    int modY1 = 107;
    int modY2 = 994;
    int modX1 = 1588;
    int modX2 = 1783;
    int modY3 = 739;
    int modY4 = 1084;
    int modX3 = 1475;
    int modX4 = 2063;

    gray.at<uchar>(modY1, modX1) = (gray.at<uchar>(modY1+1, modX1)+gray.at<uchar>(modY1-1, modX1)+gray.at<uchar>(modY1, modX1+1)+gray.at<uchar>(modY1, modX1-1)) / 4;
    gray.at<uchar>(modY2, modX2) = (gray.at<uchar>(modY2+1, modX2)+gray.at<uchar>(modY2-1, modX2)+gray.at<uchar>(modY2, modX2+1)+gray.at<uchar>(modY2, modX2-1)) / 4;
    gray.at<uchar>(modY3, modX3) = (gray.at<uchar>(modY3+1, modX3)+gray.at<uchar>(modY3-1, modX3)+gray.at<uchar>(modY3, modX3+1)+gray.at<uchar>(modY3, modX3-1)) / 4;
    gray.at<uchar>(modY4, modX4) = (gray.at<uchar>(modY4+1, modX4)+gray.at<uchar>(modY4-1, modX4)+gray.at<uchar>(modY4, modX4+1)+gray.at<uchar>(modY4, modX4-1)) / 4;
    // END OF INTERPOLATION

    // DISPLAY IMAGE
    cv::namedWindow("Display Window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Window", gray);
    cv::waitKey(0);
    // END OF DISPLAY

    // Calculate the average pixel intensity of the entire image
    cv::Scalar location_sum = cv::sum(gray);
    double location_avg = location_sum[0] / (gray.rows * gray.cols);

    int search = 0;
    for (int i = 0; i < numWorms; ++i){
        double minVal, maxVal; // Initialize location variables
        cv::Point minLoc, maxLoc;

        ++search; // Add to number of searches to avoid infinite loop

        // Create mask to remove checked areas when looking for maximum value
        cv::Mat mask = cv::Mat::ones(gray.size(), CV_8UC1) * 255;
        for (const auto& rect : checkedWorms) {
            cv::rectangle(mask, rect, cv::Scalar(0), cv::FILLED);
        }

        // Find max intensity only where a worm has not been looked for
        cv::minMaxLoc(gray, &minVal, &maxVal, &minLoc, &maxLoc, mask);

        
        double test_avg; // Initialize test variable

        // Create a 10 by 10 pixel area around the highest intensity pixel the square would go over the edge move the rectangle so it doesnt
        // Then find the average pixel intensity in the test square and add the test square to the mask so it is not tested again
        if (maxLoc.x < 4 && maxLoc.y < 4) {
            cv::Mat wormLoc = gray(cv::Rect(1, 1, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
            cv::Scalar location_test = cv::sum(wormLoc);
            test_avg = location_test[0] / (wormLoc.rows * wormLoc.cols);
            checkedWorms.push_back(cv::Rect(1, 1, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
        } else if (maxLoc.y < 4){
            cv::Mat wormLoc = gray(cv::Rect(maxLoc.x - 4, 1, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
            cv::Scalar location_test = cv::sum(wormLoc);
            test_avg = location_test[0] / (wormLoc.rows * wormLoc.cols);
            checkedWorms.push_back(cv::Rect(maxLoc.x - 4, 1, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
        } else if (maxLoc.x < 4) {
            cv::Mat wormLoc = gray(cv::Rect(1, maxLoc.y - 4, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
            cv::Scalar location_test = cv::sum(wormLoc);
            test_avg = location_test[0] / (wormLoc.rows * wormLoc.cols);
            checkedWorms.push_back(cv::Rect(1, maxLoc.y - 4, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
        } else if (maxLoc.x >= gray.cols - 4 && maxLoc.y >= gray.rows - 4) {
            cv::Mat wormLoc = gray(cv::Rect(gray.cols - 10, gray.rows - 10, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
            cv::Scalar location_test = cv::sum(wormLoc);
            test_avg = location_test[0] / (wormLoc.rows * wormLoc.cols);
            checkedWorms.push_back(cv::Rect(gray.cols - 10, gray.rows - 10, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
        } else if (maxLoc.x >= gray.cols - 4) {
            cv::Mat wormLoc = gray(cv::Rect(gray.cols - 10, maxLoc.y - 4, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
            cv::Scalar location_test = cv::sum(wormLoc);
            test_avg = location_test[0] / (wormLoc.rows * wormLoc.cols);
            checkedWorms.push_back(cv::Rect(gray.cols - 10, maxLoc.y - 4, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
        }else if (maxLoc.y >= gray.rows - 4) {
            cv::Mat wormLoc = gray(cv::Rect(maxLoc.x - 4, gray.rows - 10, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
            cv::Scalar location_test = cv::sum(wormLoc);
            test_avg = location_test[0] / (wormLoc.rows * wormLoc.cols);
            checkedWorms.push_back(cv::Rect(maxLoc.x - 4, gray.rows - 10, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
        } else {
            cv::Mat wormLoc = gray(cv::Rect(maxLoc.x - 4, maxLoc.y - 4, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
            cv::Scalar location_test = cv::sum(wormLoc);
            test_avg = location_test[0] / (wormLoc.rows * wormLoc.cols);
            checkedWorms.push_back(cv::Rect(maxLoc.x - 4, maxLoc.y - 4, 10, 10) & cv::Rect(0, 0, gray.cols, gray.rows));
        }
        
        
        // If the test average is 3 or more than than the total image average find the fluorescence
        if (test_avg - location_avg > 3) {
            int roiY = 1;
            int roiX = 1;
            int bkgY = 10;
            int bkgX = 10;
            int bkgEY = 20;
            int bkgEX = 20; // Initialize variables and size of background size

            // Set the location of the bottom of the roi and shift roi so it does not go outside the image
            if (maxLoc.y > 301 && maxLoc.y < (gray.rows - 300)) {
                roiY = maxLoc.y - 299;
            }
            else if (roiY >= (gray.rows - 300)) {
                roiY = gray.rows - 600;
            }
            if (maxLoc.y < 1) {
                roiY = 1;
            }
            if (roiY < 11) {
                bkgY = 1;
                bkgEY = 20 - (11 - roiY);
            }
            else if (roiY > (gray.rows - 611)) {
                bkgY = roiY - 10;
                bkgEY = 20 - (10 - ((gray.rows - 600) - roiY));
            }
            else {
                bkgY = roiY - 10;
                bkgEY = 20;
            }

            // Set the location of the left of the roi and shift roi so it does not go outside the image
            if (maxLoc.x > 301 && maxLoc.x < (gray.cols - 300)) {
                roiX = maxLoc.x - 300;
            }
            else if (maxLoc.x > (gray.cols - 300)) {
                roiX = gray.cols - 600;
            }
            if (roiX < 1) {
                roiX = 1;
            }
            if (roiX < 11) {
                bkgX = 1;
                bkgEX = 20 - (11 - roiX);
            }
            else if (roiX > (gray.cols - 611)) {
                bkgX = roiX - 10;
                bkgEX = 20 - (10 - ((gray.cols - 600) - roiX));
            }
            else {
                bkgX = roiX - 10;
                bkgEX = 20;
            }

            // Create rectangle object of the roi and background roi
            cv::Mat roiMain = gray(cv::Rect(roiX, roiY, roiSize, roiSize) & cv::Rect(0, 0, gray.cols, gray.rows));
            cv::Mat roiBKG = gray(cv::Rect(bkgX, bkgY, roiSize + bkgEX, roiSize + bkgEY) & cv::Rect(0, 0, gray.cols, gray.rows));

            // Sum pixel intensities inside both rois
            cv::Scalar bkg_sum = cv::sum(roiBKG);
            cv::Scalar fluo_sum = cv::sum(roiMain);
                
            // Remove roi intensities from backround and find the size of the backround to find average pixel intensity in the background
            double sumBKG = bkg_sum[0] - fluo_sum[0];
            double numBKG = (roiBKG.total() - roiMain.total());
            double meanBKG = sumBKG / numBKG;

            // Calculate the fluorescence by removing the average background intensity from each pixel in the roi
            double fluorescence = fluo_sum[0] - meanBKG * roiMain.total();

            // Check if the fluorescnce is positive and if it is save the fluorescence and add the roi to the mask
            // If it is negative look for the worm again
            if (fluorescence > 0){
                wormFluorescence.push_back(fluorescence);
                checkedWorms.push_back(cv::Rect(roiX, roiY, roiSize, roiSize));

                // DISPLAY
                cv::namedWindow("Display Window", cv::WINDOW_AUTOSIZE);
                cv::imshow("Display Window", roiMain);
                int key = cv::waitKey(0);
                // END OF DISPLAY
            }else {
                if (search <= maxSearch){
                    --i;
                }
            }
        } else {
            std::size_t numWorms = wormFluorescence.size();
            std::cout << "Looking for worm #:" << numWorms + 1 << ".\n";
        }
    }

    return 0; 
}