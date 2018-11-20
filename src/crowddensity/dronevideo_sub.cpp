 //对视频（已转换格式）做人群密度检测，并指挥无人机（未完成）
#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "std_msgs/String.h"

#include "c4-pedestrian-detector.h"
#include "tum_ardrone/detecresult.h"

using namespace std;
using namespace cv;


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
    DetecResult detecResult = mydetector(cv_bridge::toCvShare(msg, "bgr8")->image);
    //imshow("result", mydetector(cv_bridge::toCvShare(msg, "bgr8")->image).resultView);
    imshow("result", detecResult.resultView);

    //发布人群检测结果
    ros::NodeHandle n;
    ros::Publisher control_pub  = n.advertise<tum_ardrone::detecresult>("/tum_ardrone/detecresult", 1);
    tum_ardrone::detecresult msg1;
    msg1.crowdNum = detecResult.crowdNum;
    msg1.centrex = detecResult.crowdCentre.xloc;
    msg1.centrey = detecResult.crowdCentre.yloc;
    msg1.manWidth = detecResult.manWidth;
    std::cout<<msg1.crowdNum<<std::endl;
    std::cout<<msg1.centrex<<std::endl;
    std::cout<<msg1.centrey<<std::endl;
    std::cout<<msg1.manWidth<<std::endl;
    control_pub.publish(msg1);

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
  
  //订阅视频流数据
  image_transport::ImageTransport it_(nh1_);
  image_transport::Subscriber sub = it_.subscribe("/ardrone/image_raw", 1, imageCallback);


  //依人群检测结果，每隔5秒对无人机发送一次指令
  //ros::Timer timer = nh2_.createTimer(ros::Duration(5.0), controlCallBack);


  ros::spin();
  cv::destroyWindow("result");
  
  return 0;
}
