#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>

double x = 0.0, y = 0.0, th = 0.0;
void gazeboOdomGetter(const nav_msgs::Odometry::ConstPtr& msg){
    x = msg ->pose.pose.position.x;
    y = msg ->pose.pose.position.y;
    th = msg ->pose.pose.orientation.z;
}
int main(int argc, char** argv){
    ros::init(argc, argv, "odometry_publisher");
    ros::NodeHandle n;

    ros::Publisher tf_pub = n.advertise<geometry_msgs::TransformStamped>("test_tf", 50);
    ros::Subscriber odom_subscriber = n.subscribe("odom", 1000, gazeboOdomGetter);
    
    tf::TransformBroadcaster odom_broadcaster;

    ros::Time current_time, last_time;
    current_time = ros::Time::now();
    last_time = ros::Time::now();

    ros::Rate r(2.0);
    while(n.ok()){
        ros::spinOnce();
        current_time = ros::Time::now();


        geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);
        geometry_msgs::TransformStamped odom_trans;

        odom_trans.header.stamp = current_time;
        odom_trans.header.frame_id = "odom";
        odom_trans.child_frame_id = "base_link";

        odom_trans.transform.translation.x = x;
        odom_trans.transform.translation.y = y;
        odom_trans.transform.translation.z = 0.0;
        odom_trans.transform.rotation = odom_quat;

        tf_pub.publish(odom_trans);
        odom_broadcaster.sendTransform(odom_trans);

        last_time = current_time;
        r.sleep();
    }
}