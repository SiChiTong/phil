# PHIL - Position Hallucination and Indoor Localization

Full Paper: https://digitalcommons.wpi.edu/mqp-all/4087/

This project contains:

 - A library that is cross-compiler for the RIO and used in robot programs along side WPILib
 - A platform independent library for the algorithms used for localization
 - An executable that is compiled and run on the TK1 to gather sensor data and make calls to the localization algorithms
 - A collection of various executable tools and programs for testing and debugging

Make sure you run this the first time you clone

    git submodule update --init --recursive
    

## Code for the RoboRIO

If you're writing code for a roboRIO , you want to use the first one, which is a library called `phil_rio`. Since it's meant to run on the RoboRIO, not your computer, we must use a compiler that works for the RoboRIO. To get these compilers, which are a components of what is called the toolchain, [look here](http://first.wpi.edu/FRC/roborio/toolchains/). You also need to install the [WPILib eclipse plugins](http://lmgtfy.com/?q=wpilib+eclipse+plugins). Once you have that do the following.

    cd cpp
    mkdir rio_build
    cd rio_build
    cmake .. -DRIO=ON
    make
    make install

This will build `libphil_rio.a` and then install it to ~/wpilib/user/cpp so that you can use it in your Eclipse projects.


## Localization Library

This library is platform independent, and it's called `phil_common`. The point of this is so that if we have recorded data we can run it through out processing on any desktop or laptop to test and tune our algorithms. Of course this code can also run on the TK1.

## Code for the Coprocessor

The coprocessor will have a program running that aggregates the IMU + Encoder data from the RIO/NavX, reads camera images, and any other sources of snesor data. It makes sure all of these have consistent time stamps and manages any synchronization issues. it then passes sensor readings to the common processing code, gets the resulting localization information, and puts that in network tables.

## Building on Linux

Once you have all of the dependencies installed and submodules checked out, you can use a normal CMake workflow in the cpp directory

    cd cpp
    mkdir build
    cd build
    cmake ..
    make

## Building on Windows


Nope.

## Robot Datasets

Please contact Professor Michalson at WPI if you are a future MQP team and would like access to the datasets we collected. They are desrcibed breifly in our MQP paper as well.
