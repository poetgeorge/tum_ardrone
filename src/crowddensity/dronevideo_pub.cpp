//将ROS图像消息转换为OPENCV可用的图像格式

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "c4-pedestrian-detector.h"

using namespace std;
using namespace cv;

static const string OPENCV_WINDOW = "Image window";
static const string OPENCV_WINDOW2 = "Image window2";

image_transport::Subscriber image_sub_;
image_transport::Publisher image_pub_;
  
void imageCb(const sensor_msgs::ImageConstPtr& msg)
{
  cv_bridge::CvImagePtr cv_ptr;
  try
  {
   //转换图像格式
	 cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");
  }
  catch (cv_bridge::Exception& e)
  {
	 ROS_ERROR("cv_bridge exception: %s", e.what());
	 return;
  }

  Mat myimg, myimg2;
  myimg = cv_ptr->image;

  //myimg2 = mydetector(myimg);

  // Update GUI Window
  //imshow(OPENCV_WINDOW, myimg);
  //waitKey(3);

  //imshow(OPENCV_WINDOW2, myimg2);
  //waitKey(3);


  
  //发布转换后的视频
  sensor_msgs::ImagePtr msg_pub;
  msg_pub = cv_bridge::CvImage(std_msgs::Header(), "bgr8", myimg).toImageMsg();
  image_pub_.publish(msg_pub);
}

int main(int argc, char** argv)
{
	
  ros::init(argc, argv, "dronevideo_pub");
  
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_(nh_);

  //订阅原始视频，发布转换后的视频
  image_sub_ = it_.subscribe("/ardrone/image_raw", 1, imageCb);
  image_pub_ = it_.advertise("/image_converter/output_video", 1);

  //namedWindow(OPENCV_WINDOW);
  //namedWindow(OPENCV_WINDOW2);
	
  ros::spin();
  
  //destroyWindow(OPENCV_WINDOW);
  //destroyAllWindows();
  return 0;
}
