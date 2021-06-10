/*
Publish the pose of the human body wrt the map frame
 */

#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Pose.h>
#include <visualization_msgs/Marker.h>


ros::Publisher position;
geometry_msgs::Pose pose_msg;

//Subscribe to the mapper.mapperVertices
void testcallback(const visualization_msgs::Marker::ConstPtr& msg)
{
    pose_msg.position.x= msg->points[(msg->id-1)].x;
    pose_msg.position.y= msg->points[(msg->id-1)].y;
}


int main(int argc, char** argv){
    ros::init(argc, argv, "my_tf_listener");
    ros::NodeHandle node;
    //Published to estimated position
    position = node.advertise<geometry_msgs::Pose>("estimated_position", 10);

    ros::Subscriber subscrib = node.subscribe("/Mapper/vertices", 1, testcallback);

    ros::spinOnce();

    tf::TransformListener listener;
    float ang;

    ros::Rate rate(10.0);
    while (node.ok()){
        tf::StampedTransform transform;
        try{
            //Look at the tranform between the human body frame and the map frame
            listener.lookupTransform("/base_link", "/map", ros::Time(0), transform);
        }

        catch (tf::TransformException &ex) {
            ROS_ERROR("%s",ex.what());
            ros::Duration(1.0).sleep();
            continue;
        }

        pose_msg.orientation.x=transform.getRotation().x();
        pose_msg.orientation.y=transform.getRotation().y();
        pose_msg.orientation.z=transform.getRotation().z();
        pose_msg.orientation.w=-transform.getRotation().w();

        ang = 2*atan2(pose_msg.orientation.z, pose_msg.orientation.w);

        position.publish(pose_msg);
        ros::spinOnce();
        rate.sleep();
    }
    return 0;
};