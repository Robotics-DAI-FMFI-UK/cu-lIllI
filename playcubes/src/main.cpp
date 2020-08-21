/// \file main.cpp
/// This is the main program obtaining point cloud and calling inverse kinematics and then steering the arm.

#include <iostream>
#include <fstream>
#include <thread>
#include <typeinfo>

// ZED includes
#include <sl/Camera.hpp>

// OpenCV includes
#include <opencv2/opencv.hpp>

// Sample includes
//#include <SaveDepth.hpp>
//#include <GLViewer.cpp>

//PCL
#include <pcl/common/transforms.h>
#include <pcl/console/parse.h>
#include <pcl/features/normal_3d.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/filter.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/filters/radius_outlier_removal.h>
#include <pcl/filters/conditional_removal.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/vtk_lib_io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree.h>
#include <pcl/ModelCoefficients.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/sample_consensus/sac_model_sphere.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/segmentation/extract_clusters.h>
#include <pcl/segmentation/progressive_morphological_filter.h>
#include <pcl/visualization/pcl_visualizer.h>

#include <string>
#include <algorithm>
#include <iterator>
#include <string>

#include <lilli-ik.h>

using namespace sl;
using namespace std;
//using namespace std::chrono_literals;

cv::Mat slMat2cvMat(sl::Mat& input);
vector<vector<float>> coords;

/// Saves a pointcloud to a .xyz file (pointcloud format) to a specified file for later visualization.
/// (many programs can open .xyz, for instance clouudcompare)
void savePointCloud(boost::shared_ptr<pcl::PointCloud<pcl::PointXYZ>> cloud, string name){
	std::cout << "Saving clud " << name << " ..." << std::endl;
	ofstream myfile;
	name = name + ".xyz";
	myfile.open(name);
	for (std::size_t i = 0; i < cloud->points.size(); i++) {
		myfile << cloud->points[i].x << " " << cloud->points[i].y << " " << cloud-> points[i].z  << std::endl;
	}
	myfile.close();
	std::cout << "Cloud " << name << " saved ..." << std::endl;
}


/// Takes a filtered pointcloud (without the plane, and noise), and cluster them to a vector of point clouds that should correspond to individual objects.
/// Produces only clusters within minimum and maximum size (i.e. 2000 points). Outputs the poistions (centre of gravity) of the clusters to coords[] vector.
void clusterExtraction(boost::shared_ptr<pcl::PointCloud<pcl::PointXYZ>> cloud) {

  	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_f (new pcl::PointCloud<pcl::PointXYZ>);

  	// Creating the KdTree object for the search method of the extraction
 	pcl::search::KdTree<pcl::PointXYZ>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZ>);
  	tree->setInputCloud (cloud);

  	std::vector<pcl::PointIndices> cluster_indices;
  	pcl::EuclideanClusterExtraction<pcl::PointXYZ> ec;
  	ec.setClusterTolerance (2);  // in [cm]
  	ec.setMinClusterSize (2000);
  	ec.setMaxClusterSize (10000);
  	ec.setSearchMethod (tree);
  	ec.setInputCloud (cloud);
  	ec.extract (cluster_indices);

	std::vector<pcl::PointCloud<pcl::PointXYZ>, Eigen::aligned_allocator<pcl::PointXYZ> > objects;
	std::vector<float> position;

  	int j = 0;
  	for (std::vector<pcl::PointIndices>::const_iterator it = cluster_indices.begin (); it != cluster_indices.end (); ++it) {
    		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cluster (new pcl::PointCloud<pcl::PointXYZ>);
    		for (std::vector<int>::const_iterator pit = it->indices.begin (); pit != it->indices.end (); ++pit)
      			cloud_cluster->points.push_back (cloud->points[*pit]); //*
    		cloud_cluster->width = cloud_cluster->points.size ();
    		cloud_cluster->height = 1;
    		cloud_cluster->is_dense = true;
		objects.push_back(*cloud_cluster);
		savePointCloud(cloud_cluster, "figure_" + std::to_string(j));
    		std::cout << "PointCloud representing the Cluster: " << cloud_cluster->points.size () << " data points." << std::endl;
		j++;
    		
  	}
	/*
	for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
		for (std::size_t i = 0; i < iter->points.size(); i++) {
			std::cout << iter->points[i].x;
		}
	}*/

	/*Sprava pozicie*/
	float x = 0;
	float y = 0;
	float z = 0;
	
	for (auto cloud = objects.begin(); cloud != objects.end(); ++cloud) {
		for (std::size_t i = 0; i < cloud->points.size(); i++) {
			x += cloud->points[i].x;
			y += cloud->points[i].y;
			z += cloud->points[i].z;
		}
		vector<float> p;
		p.push_back(x/cloud->points.size());
		p.push_back(y/cloud->points.size());
		p.push_back(z/cloud->points.size());
		coords.push_back(p);
		x = 0;
		y = 0;
		z = 0;
	}

	
}

/// Rotates the pointcloud around the camera by the angle of the tilt of the head so that the tilt is compensated and the pointcloud
/// points will now be in the reference frame of the neck instead of the reference frame of the head. The output is sent
/// to the clusterExtraction() function.
void pointCloudRotation2(boost::shared_ptr<pcl::PointCloud<pcl::PointXYZ>> source_cloud) {
	std::cout << "Rotating point cloud..." << std::endl;
	Eigen::Affine3f transform_2 = Eigen::Affine3f::Identity();
	
	//first rotation along X axis
	transform_2.translation() << 0.0, 0.0, 0.0;

	float theta = 0.5; // angle in radians

	transform_2.rotate(Eigen::AngleAxisf(theta, Eigen::Vector3f::UnitX()));

	pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::transformPointCloud(*source_cloud, *transformed_cloud, transform_2);
	
	//savePointCloud(transformed_cloud, "2");
	std::cout << "Calling plane detection..." << std::endl;
	clusterExtraction(transformed_cloud);
}

/// Original point cloud contains some zero points (supposedly), and the points that were on the plane were replaced with zero points
/// this function creates a new pointcloud whyile removing the zero points. The output is sent to the pointCloudRotation2() function.
void zeroPointsCleaner(boost::shared_ptr<pcl::PointCloud<pcl::PointXYZ>> cloud) {
    	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>);

    	pcl::PassThrough<pcl::PointXYZ> pass;
    	pass.setInputCloud (cloud);
    	pass.setFilterFieldName ("z");
    	pass.setFilterLimits (0.0,0.0);
    	pass.setFilterLimitsNegative (true);
    	pass.filter (*cloud_filtered);
	
	//savePointCloud(cloud_filtered, "7");
    	std::cout << "Cloud cleared from blank positions ..." << std::endl;
	std::cout << "Calling final filter ..." << std::endl;
	//clusterExtraction(cloud_filtered);
	
	pointCloudRotation2(cloud_filtered);
}

/// Separates points from point cloud that lie on a plane. Plane is detected using RANSAC algorithm according to Fischler and Bolles, 1981
void planeDetection(boost::shared_ptr<pcl::PointCloud<pcl::PointXYZ>> cloud) {
	
	std::cout << "Plane detection..." << std::endl;
  	pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
  	pcl::PointIndices::Ptr inliers (new pcl::PointIndices);
  	// Create the segmentation object
  	pcl::SACSegmentation<pcl::PointXYZ> seg;
  	// Optional0
  	seg.setOptimizeCoefficients (true);
  	// Mandatory
  	seg.setModelType (pcl::SACMODEL_PLANE);
  	seg.setMethodType (pcl::SAC_RANSAC);
  	seg.setDistanceThreshold (0.5);
  	seg.setInputCloud (cloud);
  	seg.segment (*inliers, *coefficients);

  	if (inliers->indices.size () == 0)
  	{
    		PCL_ERROR ("Could not estimate a planar model for the given dataset.");
  	}

	//save detected plane
	/*ofstream ppc;
	ppc.open("5.xyz");
	//for (std::size_t i = 0; i < cloud->points.size(); i++) {
	for (std::size_t i = 0; i < inliers->indices.size(); i++) {
		ppc << cloud->points[inliers->indices[i]].x << " "
                    << cloud->points[inliers->indices[i]].y << " "
                    << cloud->points[inliers->indices[i]].z << std::endl;
	}
	ppc.close();*/

	bool save = true;
	//deleting detected plane from point cloud
	for (std::size_t j = 0; j < cloud->points.size(); j++) {
		for (std::size_t i = 0; i < cloud->points.size(); i++) {
			if (cloud->points[inliers->indices[i]].x == cloud->points[j].x &&
				cloud->points[inliers->indices[i]].y == cloud->points[j].y && 
				cloud->points[inliers->indices[i]].z == cloud->points[j].z){
				save = false;
			}			
		}
		if (!save){
			cloud->points[j].x = 0;
			cloud->points[j].y = 0;
			cloud->points[j].z = 0;	
		}
		save = true;
	}
	
	
	//savePointCloud(cloud, "6");
	std::cout << "Plane detected, calling final filter..." << std::endl;
	zeroPointsCleaner(cloud);
}

/// Removes outliers from the point cloud using point neighborhood statistics (Rusu et al. 2008) 
/// Output is sent to planeDetection() function.
void noiseDetection(boost::shared_ptr<pcl::PointCloud<pcl::PointXYZ>> cloud){
	
	std::cout << "Noise filtering..." << std::endl;
  	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>);

  	// Create the filtering object
  	pcl::StatisticalOutlierRemoval<pcl::PointXYZ> sor;
  	sor.setInputCloud (cloud);
  	sor.setMeanK (50);
  	sor.setStddevMulThresh (1);
  	sor.filter (*cloud_filtered);
	
	//savePointCloud(cloud_filtered, "4");
	std::cout << "Noise filtered, calling plane detection..." << std::endl;
	planeDetection(cloud_filtered);

}

/// Filters the pointcloud to remove the points that lie outside of the area reachable by the robot arm.
/// The output is passed to noiseDetection() function.
void pointCloudFiltering(boost::shared_ptr<pcl::PointCloud<pcl::PointXYZ>> cloud) {
	
	std::cout << "Filtering..." << std::endl;
	//making copy 
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);

	// Create the filtering object
	pcl::PassThrough<pcl::PointXYZ> pass;
	pass.setInputCloud(cloud);
	pass.setFilterFieldName("y"); //heigh		
	pass.setFilterLimits(10.0, 25.0);					
	pass.filter(*cloud_filtered);

	pass.setInputCloud(cloud_filtered);
	pass.setFilterFieldName("x");	//width
	pass.setFilterLimits(-10.0, 15.0);
	pass.filter(*cloud_filtered);

	pass.setInputCloud(cloud_filtered);
	pass.setFilterFieldName("z");	//depth
	pass.setFilterLimits(17.0, 25.0);
	pass.filter(*cloud_filtered);
	
	//savePointCloud(cloud_filtered, "3");
	std::cout << "Point cloud filtered, calling noise filtering..." << std::endl;

	noiseDetection(cloud_filtered);
	
}

/// Rotate the point cloud around camera by tilt of the head so that the coordinates are aligned with the axes of the scene in order to easily crop the reachable area.
/// The output is passed to pointCloudFiltering() function.
void pointCloudRotation(boost::shared_ptr<pcl::PointCloud<pcl::PointXYZ>> source_cloud) {
	std::cout << "Rotating point cloud..." << std::endl;
	Eigen::Affine3f transform_2 = Eigen::Affine3f::Identity();
	
	//first rotation along X axis
	transform_2.translation() << 0.0, 0.0, 0.0;

	float theta = -0.5; // angle in radians

	transform_2.rotate(Eigen::AngleAxisf(theta, Eigen::Vector3f::UnitX()));

	pcl::PointCloud<pcl::PointXYZ>::Ptr transformed_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::transformPointCloud(*source_cloud, *transformed_cloud, transform_2);
	
	//savePointCloud(transformed_cloud, "2");
	std::cout << "Calling plane detection..." << std::endl;
	pointCloudFiltering(transformed_cloud);
}

/// Convert point-cloud from the ZED format sl::Mat to PCL library format pcl::PointCloud<pcl::PointXYZ>.
/// The output is automatically sent to pointCloudRotation() function.
void pointCloudConverter(sl::Mat slPointCloud) {
	pcl::PointCloud<pcl::PointXYZ>::Ptr cvPointCloud(new pcl::PointCloud<pcl::PointXYZ>);

	cvPointCloud->width = slPointCloud.getWidth();
	cvPointCloud->height = slPointCloud.getHeight();
	cvPointCloud->points.resize(cvPointCloud->width * cvPointCloud->height);

	sl::float4 point3D;
	sl::float4 point3D2;
	int count = 0;

	std::cout << "Starting copying point cloud sl > cv" << std::endl;
	for (int i = 0; i < slPointCloud.getHeight(); i++) {
		for (int j = 0; j < slPointCloud.getWidth(); j++) {
			slPointCloud.getValue(j, i, &point3D);

			float x = point3D.x;
			float y = point3D.y;					
			float z = point3D.z;

			if (isnormal(x) && isnormal(y) && isnormal(z)) {
				if (x > -26 && x < 26) {
					cvPointCloud->points[count].x = point3D.x;
					cvPointCloud->points[count].y = point3D.y;
					cvPointCloud->points[count].z = point3D.z;
				}
			}
			count++;
		}
	}
	
	//savePointCloud(cvPointCloud, "1");
	std::cout << "Cloud copyed ..." << std::endl;
	std::cout << "Calling point cloud rotation..." << std::endl;
	pointCloudRotation(cvPointCloud);
}

/// Retrieves a point cloude from Zed-mini stereo camera using ZED SDK, it also retrieves a camera image for visualization in OpenCV window (after converttion of image format).
/// The output point cloud is passed further to the pointCloudConverter() function.
int cameraControl() {
	// Create a ZED camera object
	Camera zed;

	// Set configuration parameters
	InitParameters init_params;
	init_params.camera_resolution = RESOLUTION::HD2K;
	init_params.depth_mode = DEPTH_MODE::ULTRA;
	init_params.coordinate_units = UNIT::CENTIMETER;
	init_params.depth_minimum_distance = 15;

	// Open the camera
	ERROR_CODE err = zed.open(init_params);
	if (err != ERROR_CODE::SUCCESS) {
		printf("%s\n", toString(err).c_str());
		zed.close();
		return 1; // Quit if an error occurred
	}

	// Set runtime parameters after opening the camera
	RuntimeParameters runtime_parameters;
	runtime_parameters.sensing_mode = SENSING_MODE::FILL;	//<-presnejsi model hlbkovej mapy
	runtime_parameters.confidence_threshold = 30;			//<-vacsia presnost ale menej bodov v hlbkovej mape

	// Prepare new image size to retrieve half-resolution images
	Resolution image_size = zed.getCameraInformation().camera_resolution;
	int new_width = image_size.width / 2;
	int new_height = image_size.height / 2;

	Resolution new_image_size(new_width, new_height);

	// To share data between sl::Mat and cv::Mat, use slMat2cvMat()
	// Only the headers and pointer to the sl::Mat are copied, not the data itself
	sl::Mat image_zed(new_width, new_height, MAT_TYPE::U8_C4);
	cv::Mat image_ocv = slMat2cvMat(image_zed);
	sl::Mat point_cloud;

	// Loop until 'q' is pressed
	char key = ' ';
	while (key != 'q') {

		if (zed.grab(runtime_parameters) == ERROR_CODE::SUCCESS) {

			zed.retrieveImage(image_zed, VIEW::LEFT, MEM::CPU, new_image_size);		//<-obraz z lavej kamery
			zed.retrieveMeasure(point_cloud, MEASURE::XYZRGBA, MEM::CPU, new_image_size);	//<-point cloud

			// Display image and depth using cv:Mat which share sl:Mat data
			cv::imshow("Image", image_ocv);

			// Handle key event
			key = cv::waitKey(10);
			//processKeyEvent(zed, key);
		}
	}
	zed.close();

	std::cout << "Point cloud captured" << std::endl;
	std::cout << "Calling pointCloudConverter..." << std::endl;
	pointCloudConverter(point_cloud); //< calling converter and plane recognition
	return 0;
}

/// The main loop.
int main() {
	cameraControl();
	if (coords.size() < 1) {
		std::cout << "I see no objects." << std::endl;
		return 0; 
	}
	else if (coords.size() == 1) {
		std::cout << "I see " << coords.size() << " object." << std::endl;
		std::cout << "Coords: " << coords.at(0).at(0) << " " << coords.at(0).at(1) << " " << coords.at(0).at(2) << " " << std::endl;

		double solution[6];
		lilli_init();

   		int retval = lilli_ik(solution, -2.12, -1.41, -0.46, coords.at(0).at(0), coords.at(0).at(1), coords.at(0).at(2));
                          
   		printf("retval=%d\n", retval);
   		
		if (retval == 0) {
     			print_fk_solution(solution);
			//open port
			int serial = open("/dev/ttyUSB0", O_WRONLY);
    			if (serial < 0) printf("Not connected to arduino\n");
			if (serial > 0) {
				for (int i = 0; i < 5; i++) {
					 // if (write(serial, solution[i], strlen(cmd)) < 0) printf("error\n");
			       
			    	}
                            }
		}
		else {
			std::cout << "IK SOLUTION NOT FOUND!" << std::endl;
		}
	}
	else {
		std::cout << "I see " << coords.size() << " objects." << std::endl;
	}
	
	return 0;
}

/// Create an OpenCV wrapper around the image produces by ZED SDK (without copying the actual data - will share the same raw data).
cv::Mat slMat2cvMat(Mat& input) {
	// Mapping between MAT_TYPE and CV_TYPE
	int cv_type = -1;
	switch (input.getDataType()) {
	case MAT_TYPE::F32_C1: cv_type = CV_32FC1; break;
	case MAT_TYPE::F32_C2: cv_type = CV_32FC2; break;
	case MAT_TYPE::F32_C3: cv_type = CV_32FC3; break;
	case MAT_TYPE::F32_C4: cv_type = CV_32FC4; break;
	case MAT_TYPE::U8_C1: cv_type = CV_8UC1; break;
	case MAT_TYPE::U8_C2: cv_type = CV_8UC2; break;
	case MAT_TYPE::U8_C3: cv_type = CV_8UC3; break;
	case MAT_TYPE::U8_C4: cv_type = CV_8UC4; break;
	default: break;
	}

	// Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
	// cv::Mat and sl::Mat will share a single memory structure
	return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM::CPU));

}
