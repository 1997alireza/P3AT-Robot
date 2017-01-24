#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>

double vx = 0.0, vy = 0.0, vth = 0.0;

void cmdvelGetter(const geometry_msgs::Twist::ConstPtr& msg){
    vx = msg -> linear.x;
    //vy = msg -> linear.y; //p3at has not lateral movement
    vth = msg -> angular.z;
}
int main(int argc, char** argv){
    ros::init(argc, argv, "fake_odometry_publisher");
    ros::NodeHandle n;
    ros::Subscriber cmdvel_subscriber = n.subscribe("cmd_vel", 1000, cmdvelGetter);
    ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("fake_odom", 50);
    //ros::Publisher tf_pub = n.advertise<geometry_msgs::TransformStamped>("test_tf", 50);
    tf::TransformBroadcaster odom_broadcaster;

    double x = 0.0, y = 0.0, th = 0.0;

    ros::Time current_time, last_time;
    current_time = ros::Time::now();
    last_time = ros::Time::now();

    ros::Rate r(2.0);
    while(n.ok()){
        ros::spinOnce();
        current_time = ros::Time::now();

        double dt = (current_time - last_time).toSec();
        double delta_x = (vx*cos(th) - vy*sin(th)) * dt;
        double delta_y = (vx*sin(th) + vy*cos(th)) * dt;
        double delta_th = vth * dt;

        x += delta_x;
        y += delta_y;
        th += delta_th;

        geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);
        geometry_msgs::TransformStamped odom_trans;

        odom_trans.header.stamp = current_time;
        odom_trans.header.frame_id = "fake_odom";
        odom_trans.child_frame_id = "base_link";

        odom_trans.transform.translation.x = x;
        odom_trans.transform.translation.y = y;
        odom_trans.transform.translation.z = 0.0;
        odom_trans.transform.rotation = odom_quat;

        // tf_pub.publish(odom_trans);
        odom_broadcaster.sendTransform(odom_trans); //send the trasnform



        nav_msgs::Odometry odom;
        odom.header.stamp = current_time;
        odom.header.frame_id = "fake_odom";

        odom.pose.pose.position.x = x;
        odom.pose.pose.position.y = y;
        odom.pose.pose.position.z = 0.0;
        odom.pose.pose.orientation = odom_quat;

        odom.child_frame_id = "base_link";
        odom.twist.twist.linear.x = vx;
        odom.twist.twist.linear.y = vy;
        odom.twist.twist.angular.z = vth;

        odom_pub.publish(odom); //publish the message


        last_time = current_time;
        r.sleep();
    }
}