README.txt
==========

This README file provides instructions on how to compile the MetricCombVis plugin for MeshLab from source code.

Compiling the Plugin from Source:
---------------------------------
Compiling a plugin into MeshLab from source code requires basic knowledge of C++ programming and the use of CMake for configuration and build generation. 
Follow the steps below to compile the MetricCombVis plugin.

Prerequisites:
--------------
Ensure you have the following tools installed:
- C++ Compiler: For example, MSVC for Windows, GCC for Linux.
- CMake: Install the version appropriate for your platform.
- Qt SDK: Ensure you have a compatible version of Qt installed. For the latest release of MeshLab (MeshLab-2023.12), use Qt version 5.15.

Steps to Compile the Plugin:
----------------------------

1. **Download MeshLab Source Code**:
   - Obtain the official MeshLab source code, including all submodules, using the following command on Windows:
     ' git clone --recursive https://github.com/cnr-isti-vclab/meshlab '
     

2. **Create Directory for the Plugin**:
   - Create a directory named `filter_metricCombVis` in `meshlab\src\meshlabplugins` and copy
     the source code files (`plugin.cpp`, `plugin.h`, `CMakeLists.txt`) into this directory.

3. **Update CMakeLists.txt**:
   - Modify the `CMakeLists.txt` file in the `meshlab\src` directory.
   - Add the new plugin directory to the "# Filter plugins" section to enable CMake to identify and include the new plugin in the project build.
   - Add the following line in the appropriate section:
   	'meshlabplugins/filter_metricCombVis'

4. **Compile the Source Code and Generate the Build**:

    Follow the steps outlined in the README.md file on the MeshLab GitHub repository to compile the source code:
    https://github.com/cnr-isti-vclab/meshlab/blob/main/src/README.md


5. **Open and Compile the Project**:

    Open the generated build project in an Integrated Development Environment (IDE) such as Visual Studio or Qt Creator.
    Compile the project. During this step, you can choose to generate either the final release version or the debug version.
    The resulting DLL file for the plugin will be located in the meshlab\build\src\distrib\plugins directory for Windows
    (on other platforms, the DLL will be in the build directory under the plugins section).


6. **Load the Plugin**:

    Please read the README files in the 'release' and 'debug' folders. These files contain procedures for inserting the plugin into the MeshLab application.

    