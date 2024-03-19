#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>
#include <filesystem>

#include <Tracker.hpp>

#include <TUIOServer.hpp>

int main() {

    cv::VideoCapture capture("figures/video.AVI");

    if(!capture.isOpened()){
        std::cerr << "Error: Unable to open Video file.\n";
        return EXIT_FAILURE;
    }


    cv::Mat background;
    if(!capture.read(background)) {
        std::cerr << "Error: Unable to read Video file.\n";
        return EXIT_FAILURE;
    }

    Tracker tracker(background);

    cv::Mat vid_frame;
    while(capture.read(vid_frame)){
        tracker.update(vid_frame);
        cv::imshow("Output",vid_frame + tracker.drawInfo());
        if(cv::waitKey(100) == 27) break;
    }

    capture.release();
    cv::destroyAllWindows();

    return EXIT_SUCCESS;
}

