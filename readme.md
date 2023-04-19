# Operation Instructions

LS Lidar linux and windows system c++ demo

## Dependance

pcl library ： sudo apt-get install libpcl-dev

refer to:https://github.com/PointCloudLibrary/pcl

## Compiling and running

Compiling process under Linux system
Switch to the directory that the driver folder belongs to
mkdir build && cd build
cmake ..
make -j4

Obtain data：demo： ./demo

visual demo： ./demo_viewer



Compiling process under Windows system：
Open cmake-gui, set the search path of PCL library and compile directly

or use Visual Studio to compile
