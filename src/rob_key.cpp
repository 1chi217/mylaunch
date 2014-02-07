#include <ros/ros.h>
#include "geometry_msgs/Twist.h"

#include <signal.h>
#include <termios.h>
#define KEYCODE_R 0x43
#define KEYCODE_L 0x44
#define KEYCODE_U 0x41
#define KEYCODE_D 0x42
#define KEYCODE_Q 0x71
#define KEY_SPACE 0x20


using geometry_msgs::Twist;
using namespace std;

ros::Publisher chatter_pub;
ros::Time t1;
Twist vel;
int kfd = 0;
struct termios cooked, raw;
void quit(int sig)
{
    tcsetattr(kfd, TCSANOW, &cooked);
    ros::shutdown();
    exit(0);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "Rob_key");
    ros::NodeHandle n;
    chatter_pub = n.advertise<Twist>("/RosAria/cmd_vel", 1);
    signal(SIGINT,quit);
    char c;
    bool dirty=false;

    t1=ros::Time::now();

    tcgetattr(kfd, &cooked);
    memcpy(&raw, &cooked, sizeof(struct termios));
    raw.c_lflag &=~ (ICANON | ECHO);
    raw.c_cc[VEOL] = 1;
    raw.c_cc[VEOF] = 2;
    tcsetattr(kfd, TCSANOW, &raw);

    while (ros::ok())
    {
        if(read(kfd, &c, 1) < 0)
        {
            perror("read():");
            exit(-1);
        }
        switch(c)
        {
        case KEYCODE_L:
            ROS_DEBUG("LEFT");
            puts("TURN LEFT");
            vel.angular.z  = 0.3;
            dirty = true;
            break;

        case KEYCODE_R:
            ROS_DEBUG("RIGHT");
            puts("TURN RIGHT");
            vel.angular.z  = -0.3;
            dirty = true;
            break;

        case KEYCODE_U:
            ROS_DEBUG("UP");
            puts("FORWARD");
            vel.linear.x = 0.2;
            vel.angular.z  = 0;
            dirty = true;
            break;
        case KEYCODE_D:
            ROS_DEBUG("DOWN");
            puts("BACKWARD");
            vel.linear.x = -0.2;
            vel.angular.z  = 0;
            ROS_DEBUG("LEFT");
            dirty = true;
            break;

        case KEY_SPACE:
            ROS_DEBUG("STOP");
            puts("SPACE");
            vel.linear.x = 0.0;
            vel.angular.z = 0.0;
            dirty = true;
        }

        vel.linear.y=0;
        vel.linear.z=0;
        vel.angular.x = 0;
        vel.angular.y = 0;
        if(dirty==true)
        {

            chatter_pub.publish(vel);
            dirty=false;

        }

        ros::spinOnce();

    }//while
    return(0);
}
