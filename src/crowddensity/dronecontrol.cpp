#include <ros/ros.h>
#include "std_msgs/String.h"

#include "c4-pedestrian-detector.h"
#include "tum_ardrone/detecresult.h"

static int crowdNum = 0; //人数
static int compareNum = 0;
static double manWidth = 0.0; //平均人体宽度
static double compareWidth = 0.0;
static CrowdLocation crowdCentre = CrowdLocation(); //人群中心点

void controlCB(const ros::TimerEvent&){
    ros::NodeHandle n;
    ros::Publisher control_pub = n.advertise<std_msgs::String>("/tum_ardrone/com", 1000);
    std_msgs::String msg_left;
          std::stringstream ss_left;
          ss_left << "c goto -1 0 0 0";
          msg_left.data = ss_left.str();
    std_msgs::String msg_right;
          std::stringstream ss_right;
          ss_right << "c goto 1 0 0 0";
          msg_right.data = ss_right.str();
    if(crowdCentre.xloc > 0){
        ROS_INFO("Fly right! Aha!");
        control_pub.publish(msg_right);
    }
    else if(crowdCentre.xloc < 0){
        ROS_INFO("Fly lift! Aha!");
        control_pub.publish(msg_left);
    }
    compareWidth = manWidth;
    compareNum = crowdNum;
}


void mainCB(const tum_ardrone::detecresult::ConstPtr& msg){
    ROS_INFO("controlCallback triggered");
    crowdNum = msg->crowdNum;
    crowdCentre = CrowdLocation(msg->centrex, msg->centrey);
    manWidth = msg->manWidth;

    ros::NodeHandle nc;
    //依人群检测结果，每隔5秒对无人机发送一次指令
    ros::Timer timer = nc.createTimer(ros::Duration(5.0), controlCB);
}

int main(int argc, char** argv){
    ros::init(argc, argv, "mycontroller");
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("detecresult", 1000, mainCB);
    ros::spin();
    return 0;
}