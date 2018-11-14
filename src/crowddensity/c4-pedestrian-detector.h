#ifndef C4_PEDESTRIAN_DETECTOR_H_
#define C4_PEDESTRIAN_DETECTOR_H_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <string>
#include <cmath>
#include <vector>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class CrowdLocation{
    public:
    double xloc;
    double yloc;
    public:
    CrowdLocation();
    CrowdLocation(double x, double y);
};

// CrowdLocation::CrowdLocation(){
// }

// CrowdLocation::CrowdLocation(double x, double y){
// }

class DetecResult{
    public:
    CrowdLocation crowdCentre; //人群中心点
    int crowdNum; //人数
    cv::Mat resultView; //框出了人群的图像
    double manWidth; //人体框平均宽度，以此判定人群相对距离；
    public:
    DetecResult(CrowdLocation cl, int num, cv::Mat rv, double mw);
};

// DetecResult::DetecResult(CrowdLocation cl, cv::Mat rv){
// }

DetecResult mydetector(cv::Mat myimg);
//cv::Mat mydetector(cv::Mat myimg);

#endif