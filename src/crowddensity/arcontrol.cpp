#include <ros/ros.h>
#include <iostream>
#include <string>
#include <cmath>
#include <unistd.h>
#include "cvd/thread.h"
#include "ros/callback_queue.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
//#include <linux/delay.h>
#include <sys/time.h>

#include "c4-pedestrian-detector.h"
#include "tum_ardrone/detecresult.h"

static int crowdNum = 0; //人数
static int compareNum = 0;
static double manWidth = 0.0; //最大人体宽度
static double compareWidth = 0.0;
static CrowdLocation crowdCentre = CrowdLocation(); //人群中心点

// struct timeval{
//     long tv_sec;
//     long tv_usec;
// };

class Sub_Pub_Timer{
    private:
    ros::NodeHandle n_;
    ros::Publisher pub_;
    ros::Subscriber sub_;
    ros::Timer timer_;
    public:
    Sub_Pub_Timer(){
        pub_ = n_.advertise<std_msgs::String>(n_.resolveName("tum_ardrone/com"), 50);
        sub_ = n_.subscribe(n_.resolveName("/detecresult"), 50, &Sub_Pub_Timer::subCB, this);
        timer_ = n_.createTimer(ros::Duration(2.0), &Sub_Pub_Timer::timerCB, this);
    }
    void subCB(const tum_ardrone::detecresult::ConstPtr& msg){
        ROS_INFO("controlCallback triggered");
        //std::cout<<"triggered"<<std::endl;
        crowdNum = msg->crowdNum;
        crowdCentre = CrowdLocation(msg->centrex, msg->centrey);
        manWidth = msg->manWidth;
    }
    void timerCB(const ros::TimerEvent&){
        const int N = 500000000;
        int i=0,j=0;
        pthread_mutex_t send_CS = PTHREAD_MUTEX_INITIALIZER;
        double dist_lr = 2.5 * crowdCentre.xloc; //左或右平移距离
        if(dist_lr >= 2.0){
            dist_lr = 1.9;
        }
        if(dist_lr <= -2.0){
            dist_lr = -1.9;
        }
        std::stringstream ss_lr;
        ss_lr<<dist_lr;
        std::string strdist_lr = ss_lr.str();
        std::string cmd_lr = "c moveBy " + strdist_lr + " 0 0 0"; //向左或右飞行
        std::string cmd_ref = "c setReference 0 0 0 0";
        std_msgs::String msg_lr;
        std_msgs::String msg_ref;
        msg_lr.data = cmd_lr.c_str();
        msg_ref.data = cmd_ref.c_str();
        double dist_fb = 0.5; //前进或后退距离
        std::stringstream ss_fore;
        ss_fore<<dist_fb;
        std::string strdist_fore = ss_fore.str();
        std::string cmd_lrf = "c goto " + strdist_lr + ' ' + strdist_fore + " 0 0"; //向左前或右前飞行
        std_msgs::String msg_lrf;
        msg_lrf.data = cmd_lrf.c_str();
        std::stringstream ss_back;
        ss_back<<(-dist_fb);
        std::string strdist_back = ss_back.str();
        std::string cmd_lrb = "c goto " + strdist_lr + ' ' + strdist_back + " 0 0"; //向左后或右后飞行
        std_msgs::String msg_lrb;
        msg_lrb.data = cmd_lrb.c_str();
        std::string resetf = "f reset"; 
        std_msgs::String msgrs;
        msgrs.data = resetf.c_str();
        // std_msgs::String msg_left;
        // std::string ss_left = "c goto -0.5 0 0 0";
        // msg_left.data = ss_left.c_str();
        // std_msgs::String msg_right;
        // std::string ss_right = "c goto 0.5 0 0 0";
        // msg_right.data = ss_right.c_str();
        // std::cout<<crowdNum<<std::endl;
        
        //检测到人
        if(crowdNum>0){
            // if(manWidth > (2*compareWidth)){
            //     printf("fly forward, lr %f", dist_lr);
            //     pthread_mutex_lock(&send_CS);
            //     pub_.publish(msg_lrf);
            //     pthread_mutex_unlock(&send_CS);
            // }
            // else if(manWidth < (0.5*compareWidth)){
            //     printf("fly backward, lr %f", dist_lr);
            //     pthread_mutex_lock(&send_CS);
            //     pub_.publish(msg_lrb);
            //     pthread_mutex_unlock(&send_CS);
            // }
            // else{
                ROS_INFO("fly left_right %f", dist_lr);
                //printf("fly left_rgiht %f", dist_lr);
                pthread_mutex_lock(&send_CS);
                // struct timeval t1, t2;
                // double t3;
                // gettimeofday(&t1, NULL);
                // pub_.publish(msgrs);  //按人群中心点比例控制飞机左或右平飞
                // gettimeofday(&t2, NULL);
                // t3 = 1000000 * (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec);
                // printf("reset time %f\n", t3);
                // //mdelay(500);
                // for(i=0;i<N;i++){
                //     j = i;
                // }
                // pthread_mutex_unlock(&send_CS);
                // pthread_mutex_lock(&send_CS);
                pub_.publish(msg_lr);
                ROS_INFO("fly good");
                // for(int i=0;i<5000;i++){}
                // pub_.publish(msgrs);
                pthread_mutex_unlock(&send_CS);
            //}
        }
        //未检测到人
        else{
            ROS_INFO("NONONONONO,No human detected!");
            std::string cmd_scan = "c moveBy 0 0 0 15";
            std_msgs::String msg_scan;
            msg_scan.data = cmd_scan.c_str();
            pthread_mutex_lock(&send_CS);
            pub_.publish(msg_scan); //飞机向右转15度继续监测
            ROS_INFO("Scanning......");
            pthread_mutex_unlock(&send_CS);
            //std::cout<<"NONONONONO,No human detected!"<<std::endl;
            
        }
        compareWidth = manWidth;
        compareNum = crowdNum;
    }
};

int main(int argc, char** argv){
    ros::init(argc, argv, "mycontroller");
    Sub_Pub_Timer SPTObject;
    ros::spin();
    return 0;
}