 //对视频（已转换格式）做人群密度检测，并指挥无人机（未完成）
#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "std_msgs/String.h"

#include "c4-pedestrian-detector.h"

using namespace std;
using namespace cv;

static int crowdNum = 0; //人数
static int compareNum = 0;
static double manWidth = 0.0; //平均人体宽度
static double compareWidth = 0.0;
static CrowdLocation crowdCentre = CrowdLocation(); //人群中心点

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  //cv_bridge::CvImageConstPtr cv_ptr;
  //cv::Mat myimg, myimg2;
  try
  {
    cv_bridge::CvImagePtr cv_ptr;
    cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");
    Mat myimg;
    myimg = cv_ptr->image;
    //输出人群检测结果
    //imshow("result", mydetector(cv_bridge::toCvShare(msg, "bgr8")->image).resultView);
    imshow("result", mydetector(myimg).resultView);

    //crowdNum = mydetector(cv_bridge::toCvShare(msg, "bgr8")->image).crowdNum;
    //crowdCentre = mydetector(cv_bridge::toCvShare(msg, "bgr8")->image).crowdCentre;
    crowdNum = mydetector(myimg).crowdNum;
    crowdCentre = mydetector(myimg).crowdCentre;

    waitKey(100);   //100ms
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}

//指挥无人机
void controlCallBack(const ros::TimerEvent&)
{
  ROS_INFO("controlCallback triggered");
  // ros::NodeHandle n;
  // ros::Publisher control_pub = n.advertise<std_msgs::String>("/tum_ardrone/com", 1000);
  // std_msgs::String msg_left;
  //       std::stringstream ss_left;
  //       ss_left << "c goto -0.5 0 0 0";
  //       msg_left.data = ss_left.str();
  // std_msgs::String msg_right;
  //       std::stringstream ss_right;
  //       ss_right << "c goto 0.5 0 0 0";
  //       msg_right.data = ss_right.str();
  
  // if(crowdCentre.xloc > 0.2){
  //   control_pub.publish(msg_right);
  // }
  // else if(crowdCentre.xloc < -0.2){
  //   control_pub.publish(msg_left);
  // }
  // compareWidth = manWidth;
  // compareNum = crowdNum;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "dronevideo_sub");
  ros::NodeHandle nh1_, nh2_;
  cv::namedWindow("result");
  cv::startWindowThread();
  
  //发布人群密度检测结果
  image_transport::ImageTransport it_(nh1_);
  //image_transport::Subscriber sub = it_.subscribe("/image_converter/output_video", 1, imageCallback);
  image_transport::Subscriber sub = it_.subscribe("/ardrone/image_raw", 1, imageCallback);


  //依人群检测结果，每隔5秒对无人机发送一次指令
  ros::Timer timer = nh2_.createTimer(ros::Duration(5.0), controlCallBack);

  ros::spin();
  cv::destroyWindow("result");
}
