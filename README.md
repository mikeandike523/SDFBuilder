# SDFBuilder

*Generate Signed Distance Fields with STL files*

### Features

- Import STL files with a simple drag-and drop.
- Visualize STL files with eye-pleasing smooth shading.
- Generate signed distance fields, and export as volumetric data.

### Examples

<table>
<tr>
<td><img src="https://github.com/mikeandike523/SDFBuilder/blob/main/ReadmeFiles/screenshot_knight.png?raw=true" width="256px" height="192px"/></td>
<td><img src="https://github.com/mikeandike523/SDFBuilder/blob/main/ReadmeFiles/screenshot_helios.png?raw=true" width="256px" height="192px"/></td> 
</tr>
</table>

### Usage

##### Requirements

- Windows 10 PC with Intel I7 processor, and Nvidia GeForce 1060 or 1080.

##### Installation

- Download and run the installer for SDFBuilder 0.1 from its [Release Page](https://github.com/mikeandike523/SDFBuilder/releases/tag/0.1).

##### Converting and Visualizing Files

- Run the program from the windows start menu or search bar.
- Drag and drop an STL file from any location into the application window.
- Use the live visualization to track conversion progress. A dialog will popup when conversion is complete.
- Locate the output SDF file in the STL source folder (e.g. `desktop/Knight.stl→desktop/Knight.sdf`).
- Alternatively, you can drag an already-processed .sdf file onto the window for instant visualization.

##### Output File Format

- Output files are stored with extension `.sdf`, using a modified version of Paule Bourke's [Volumetric Data Format](http://paulbourke.net/dataformats/volumetric/#:~:text=Volume%20data%20format&text=A%20bit%20like%20the%20PPM,as%20having%20more%20header%20fields.) spec. For SDFBuilder, the output file's binary region consists of 4 byte floats, instead of signed integers.
- The Y and Z coordinates of the initial model are swapped.
- The model is centered at the origin, and scaled such that its smallest dimension is 2 units with .4 units of padding.

## :warning: PC TEMPERATURE WARNING

- This software is in an experimental stage, and is extremely taxing on your CPU and GPU. When using this software, watch for temperature increases in your CPU and GPU, as well as hot computer surfaces. The author of this repository, github user mikeandike523, is not liable for any damages to hardware or person. Use at your own risk. 

### Acknowledgements

- Inigo Quilez, for providing GLSL algorithms for the explicit signed distance functions used in this program, as well as an algorithm for smooth minimum. See articles [Signed Distance functions](https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm), [Distance to Triangle](https://www.iquilezles.org/www/articles/triangledistance/triangledistance.htm), and [Smooth Minimum](https://www.iquilezles.org/www/articles/smin/smin.htm).
- Thingiverse users dosse91 and Miquel Adell for the [Helios](https://www.thingiverse.com/thing:2887298) and [chess knight](https://www.thingiverse.com/thing:3077961) example models.

### License

- This codebase is released under the [GNU GPL 3.0](https://www.gnu.org/licenses/gpl-3.0.en.html) license. 





