# SDFBuilder

*Generate Signed Distance Fields with STL files*

### Features

- Import STL files with a simple drag-and drop.
- Visualize STL files with eye-pleasing smooth shading
- Generate signed distance fields, and export [Paule Bourke](http://paulbourke.net/dataformats/volumetric/#:~:text=Volume%20data%20format&text=A%20bit%20like%20the%20PPM,as%20having%20more%20header%20fields.) volumetric data

### Examples

| ![](https://github.com/mikeandike523/SDFBuilder/blob/main/ReadmeFiles/screenshot_knight.png?raw=true) |
| :----------------------------------------------------------: |
|            ![](https://github.com/mikeandike523/SDFBuilder/blob/main/ReadmeFiles/screenshot_helios.png?raw=true)            |

### Usage

This project is in a prelease state, and is not yet available as a standalone application. The project can be built from source, given the requirements and steps below.

##### Requirements

- Windows 10 PC with Intel I7 processor, and Nvidia GeForce 1060 or 1080.
- Microsoft Visual Studio 2019

##### Build Process

- Clone or download this repo

  ```
  git clone https://github.com/mikeandike523/SDFBuilder
  ```

- Open `SDFBuilder/SDFBuilder.sln` with Visual Studio 2019

- Select  the `Debug, x86` build configuration in the Visual Studio toolbar

- Select `Build→Build Solution` in the Visual Studio toolbar

##### Converting and Visualizing Files

- Run the project by selecting `Debug→Start without debugger` in the visual studio toolbar
- Drag and drop an STL file from any location into the application window
- Use the live visualization to track conversion progress
- Locate the output SDF file in the STL source folder (e.g. `desktop/Knight.stl→desktop/Knight.sdf`)

##### Output File Format

- Output files are stored with extension `.sdf`, using [Paule Bourke's](http://paulbourke.net/dataformats/volumetric/#:~:text=Volume%20data%20format&text=A%20bit%20like%20the%20PPM,as%20having%20more%20header%20fields.) volumetric data format
- The Y and Z coordinates of the initial model are swapped
- The model is centered at the origin, and scaled such that its smallest dimension is 2 units

### Caution

This software is highly taxing on your CPU and GPU, and can cause temperature increases in these processors. Ensure your PC protects against temperature spikes, and watch for hot computer surfaces (especially with laptops).

### Acknowledgements

- Inigo Quilez, for providing GLSL algorithms for the explicit signed distance functions used in this program, as well as an algorithm for smooth minimum. See articles [Signed Distance functions](https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm), [Distance to Triangle](https://www.iquilezles.org/www/articles/triangledistance/triangledistance.htm), and [Smooth Minimum](https://www.iquilezles.org/www/articles/smin/smin.htm).
- Thingiverse users dosse91 and Miquel Adell for the [Helios](https://www.thingiverse.com/thing:2887298) and [chess knight](https://www.thingiverse.com/thing:3077961) example models.
- Various StackOverflow users for C++ guidance, though no code was directly copied

### License

This code is released under the [GNU GPL 3.0](https://www.gnu.org/licenses/gpl-3.0.en.html) license. 





