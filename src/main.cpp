#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[])
{
    std::cout << cv::getBuildInformation() << std::endl;
}
