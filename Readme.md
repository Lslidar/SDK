# Operation Instructions

LSLiDAR Linux and Windows system C++ Demo

# Dependence

PCL library: sudo apt-get install libpcl-dev

refer to: https://github.com/PointCloudLibrary/pcl

# Compiling and Running

#### Compiling process under Linux system

Switch to the directory that the driver folder belongs to

mkdir build & cd build

cmake -j4

Obtain data: deme: ./demo

visual demo: ./demo_viewer

#### Compiling under Windows System

Open cmake-gui, set the search path of PCL library and compile directly or use Visual Studio to compile.

<img src="https://github.com/Lslidar/Lslidar_ROS1_driver/blob/master/image/LS180S1.png" width="100px" />  [link](https://github.com/Lslidar/SDK/tree/C1_V4.0)
