README.txt
==========

This README file provides an overview of the files included in the `mesh_object` directory 
and instructions on how to use these files for testing the MetricCombVis plugin in MeshLab.

Directory: mesh_object
-----------------------
The `mesh_object` directory contains selected mesh files for testing the MetricCombVis plugin. 
These files are provided in the `.ply` format and can be used to evaluate the plugin's functionality.

Files Included:
---------------
1. bunny-changed.ply 
2. bunny-original.ply 
3. happy_budha-changed.ply 
4. happy_budha-original.ply 
5. tetrahedron.ply

Using the Sample 3D Model Files:
--------------------------------
The sample 3D model files included in this directory are intended for use with the MetricCombVis plugin in MeshLab. 
Follow the instructions below to load and use these files:

1. **Open MeshLab**:
   Ensure you have the latest version of MeshLab installed on your system. The `filter_metricCombVis` plugin should also be installed and loaded.

2. **Load a Sample Mesh**:
   - Open MeshLab.
   - Navigate to `File` -> `Import Mesh`.
   - Browse to the `mesh_object` directory and select one of the sample `.ply` files (e.g., `bunny-original.ply`).

3. **Use the MetricCombVis Plugin**:
   - With the sample mesh loaded, go to the `Filters` -> `Quality Measurement` menu.
   - Select one of the visualization options provided by the `MetricCombVis` plugin:
     - **First Visualization**: This option will process the loaded 3D model and display the first type of visualization.
     - **Second Visualization**: This option will process the loaded 3D model and display the second type of visualization.

4. **Evaluate the Plugin**:
   - Observe the results of the plugin's processing on the sample mesh.
   - Use the different sample files (`bunny-original.ply`, `happy_budha-original.ply`...) to test the plugin's functionality and performance on various types of 3D models.


