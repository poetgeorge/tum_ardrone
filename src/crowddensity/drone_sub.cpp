#include <ros/ros.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "std_msgs/String.h"

#include "c4-pedestrian-detector.h"
#include "tum_ardrone/detecresult.h"

//using namespace std;
using namespace cv;

class Sub_Pub{
    private:
    ros::NodeHandle nh1_;
    //image_transport::ImageTransport it_(nh1_);
    image_transport::Subscriber sub;
    ros::Publisher control_pub;
    public:
    Sub_Pub(){
        image_transport::ImageTransport it_(nh1_);
        sub = it_.subscribe("/ardrone/image_raw", 50, &Sub_Pub::imageCallback, this);
        control_pub = nh1_.advertise<tum_ardrone::detecresult>("detecresult", 50);
    }
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
    //ros::NodeHandle n;
    //ros::Publisher control_pub  = nh1_.advertise<tum_ardrone::detecresult>("detecresult", 50);
    tum_ardrone::detecresult msg1;
    msg1.crowdNum = detecResult.crowdNum; //人数
    msg1.centrex = detecResult.crowdCentre.xloc; //人群中心点
    msg1.centrey = detecResult.crowdCentre.yloc;
    msg1.manWidth = detecResult.manWidth; //最大人体宽度
    ROS_INFO("num: %d; centre: %f, %f; width: %f", msg1.crowdNum, msg1.centrex, msg1.centrey, msg1.manWidth);
    // std::cout<<msg1.crowdNum<<std::endl;
    // std::cout<<msg1.centrex<<std::endl;
    // std::cout<<msg1.centrey<<std::endl;
    // std::cout<<msg1.manWidth<<std::endl;
    control_pub.publish(msg1);

    waitKey(300);   //300ms
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}
};
int main(int argc, char **argv)
{
  ros::init(argc, argv, "dronevideo_sub");
  //ros::NodeHandle nh1_;
  cv::namedWindow("result");
  cv::startWindowThread();

  Sub_Pub SPobject;
  
  //订阅视频流数据
//   image_transport::ImageTransport it_(nh1_);
//   image_transport::Subscriber sub = it_.subscribe("/ardrone/image_raw", 50, imageCallback);


  //依人群检测结果，每隔5秒对无人机发送一次指令
  //ros::Timer timer = nh2_.createTimer(ros::Duration(5.0), controlCallBack);


  ros::spin();
  cv::destroyWindow("result");
  
  return 0;
}