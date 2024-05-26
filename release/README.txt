README.txt
==========

This README file provides instructions on how to integrate and use the MetricCombVis plugin in MeshLab.

Files Included:
---------------
1. metricCombVis.dll  - The compiled plugin binary.


Prerequisites:
--------------
1. MeshLab - Ensure that you have the latest version of MeshLab installed on your system from the `.exe` installer (for Windows).

Instructions to Install MetricCombVis Plugin:
-----------------------------------------------------
You can load the MetricCombVis plugin into MeshLab using one of the following methods:

### Method 1: Using the Plugin Info Dialog
1. Open MeshLab.
2. Navigate to `Help` -> `Plugin Info`.
3. In the Plugin Info dialog, click on the `Load Plugin` button.
4. Browse to the location of the `filter_metricCombVis` plugin file (e.g., `plugin.dll`, `plugin.so`, or `plugin.dylib`) and select it.
5. The plugin should now be loaded into MeshLab, and you can start using it.

### Method 2: Copying to the Plugins Directory
1. Locate the MeshLab plugins directory. This is typically located in the installation directory of MeshLab:
   - Windows: `C:\Program Files\MeshLab\plugins`
   - macOS: `/Applications/MeshLab.app/Contents/plugins`
   - Linux: `/usr/local/share/meshlab/plugins` or `/usr/share/meshlab/plugins`
2. Copy the `filter_metricCombVis` plugin file (e.g., `plugin.dll`, `plugin.so`, or `plugin.dylib`) to the plugins directory.
3. Restart MeshLab.
4. The plugin should now be available in the MeshLab interface, and you can start using it.


--------------------------------------------------------------
How to Use plugin:

1. Open MeshLab.
2. Load mesh object by navigating to `File` -> `Import Mesh`.
3. Use the `filter_metricCombVis` plugin to process the loaded model 
by selecting the appropriate visualization option from the `Filters` -> `Quality Measurement` menu.


Enjoy using the `MetricCombVis plugin with MeshLab!
