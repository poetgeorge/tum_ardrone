#include <ros/ros.h>
#include <iostream>
#include <string>
#include <cmath>
#include <unistd.h>
#include "cvd/thread.h"
#include "ros/callback_queue.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"

#include "c4-pedestrian-detector.h"
#include "tum_ardrone/detecresult.h"


int main(int argc, char** argv){
    ros::init(argc, argv, "my_takeoff");
    ros::NodeHandle n;
    ros::Publisher puber = n.advertise<std_msgs::String>(n.resolveName("tum_ardrone/com"), 50);
    std::string takeoff = "c takeoff";
    std::string autoinit = "c autoInit 500 800 4000 0.5";
    std::string climb = "c goto 0 0 1.0 0";
    std_msgs::String msg_takeoff;
    std_msgs::String msg_init;
    std_msgs::String msg_climb;
    msg_takeoff.data = takeoff.c_str();
    msg_init.data = autoinit.c_str();
    msg_climb.data = climb.c_str();
    pthread_mutex_t send_CS = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&send_CS);
    puber.publish(msg_takeoff);
    puber.publish(msg_init);
    puber.publish(msg_climb);
    pthread_mutex_unlock(&send_CS);
}