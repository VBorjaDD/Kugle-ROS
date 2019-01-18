//package name, header name for new plugin library
#include <kugle_mpc/kugle_mpc_plugin.h>

#include <pluginlib/class_list_macros.h>

#include <nav_msgs/Path.h>

PLUGINLIB_EXPORT_CLASS(KugleMPC, nav_core::BaseLocalPlanner);

KugleMPC::KugleMPC(){
	//nothing ot fill in here; "initialize" will do the initializations
}

//put inits here:
void KugleMPC::initialize(std::string name, tf::TransformListener * tf, costmap_2d::Costmap2DROS * costmap_ros){
	ros::NodeHandle nh(name);
	
	old_size = 0;
	handed_tf = tf;
	costmap_handle = costmap_ros;

	pub_plan = nh.advertise<nav_msgs::Path>("global_plan", 1000);
}

bool KugleMPC::isGoalReached(){
	//For demonstration purposes, sending a single navpoint will cause five seconds of activity before exiting.
	return ros::Time::now() > tg;
}

bool KugleMPC::setPlan(const std::vector< geometry_msgs::PoseStamped > &plan){
	//The "plan" that comes in here is a bunch of poses of varaible length, calculated by the global planner(?).
	//We're just ignoring it entirely, but an actual planner would probably take this opportunity
	//to store it somewhere and maybe update components that refrerence it.
	ROS_INFO("GOT A PLAN OF SIZE %lu", plan.size());
	//If we wait long enough, the global planner will ask us to follow the same plan again. This would reset the five-
	//second timer if I just had it refresh every time this function was called, so I check to see if the new plan is
	//"the same" as the old one first.
	if(plan.size() != old_size){
		old_size = plan.size();
		tg = ros::Time::now() + ros::Duration(5.0);
	}

	nav_msgs::Path planMsg;
	planMsg.header.stamp = ros::Time::now();
	planMsg.header.frame_id = "odom";
	planMsg.poses.insert(planMsg.poses.begin(), plan.begin(), plan.end());
	pub_plan.publish(planMsg);
	
	return true;
}

bool KugleMPC::computeVelocityCommands(geometry_msgs::Twist &cmd_vel){
	//This is the meat-and-potatoes of the plugin, where velocities are actually generated.
	//in this minimal case, simply specify constants; more generally, choose vx and omega_z
	// intelligently based on the goal and the environment
	// When isGoalReached() is false, computeVelocityCommands will be called each iteration
	// of the controller--which is a settable parameter.  On each iteration, values in
	// cmd_vel should be computed and set, and these values will be published by move_base
	// to command robot motion
	cmd_vel.linear.x = 0.2;
	cmd_vel.linear.y = 0.0;
	cmd_vel.linear.z = 0.0;
        cmd_vel.angular.z = 0.2;
	return true;
}
