README.txt
==========

This README file provides instructions on how to integrate and use the debug version of the MetricCombVis plugin in MeshLab when MeshLab is built from source.

Note: This plugin is intended for use with debug versions of MeshLab.


Prerequisites:
--------------
1. Debug version of MeshLab.


Instructions to Load and Use the Debug Plugin:
---------------------------------------------
You can load the debug version of the MetricCombVis plugin into MeshLab using following method:

### Method: Using the Plugin Info Dialog
1. Open the debug build of MeshLab.
2. Navigate to `Help` -> `Plugin Info`.
3. In the Plugin Info dialog, click on the `Load Plugin` button.
4. Browse to the location of the `debug metricCombVis.dll` file and select it.
5. The debug version of the plugin should now be loaded into MeshLab, and you can start using it.


Using the `MetricCombVis Plugin:
---------------------------------------
The MetricCombVis plugin adds two new functionalities under the "Filters" -> "Quality Measurement" menu:

1. **First Visualization**:
   - To use the first visualization feature:
     1. Load a 3D model file by navigating to `File` -> `Import Mesh` and selecting one of the provided sample files (e.g., `bunny-original.ply`).
     2. Go to `Filters` -> `Quality Measurement` -> `First Visualization`.
     3. The plugin will process the loaded 3D model and display the first type of visualization.

2. **Second Visualization**:
   - To use the second visualization feature:
     1. Load a 3D model file by navigating to `File` -> `Import Mesh` and selecting one of the provided sample files (e.g., `happy_budha-original.ply`).
     2. Go to `Filters` -> `Quality Measurement` -> `Second Visualization`.
     3. The plugin will process the loaded 3D model and display the second type of visualization.
