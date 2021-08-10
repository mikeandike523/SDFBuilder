# SDFBuilder

*Generate Signed Distance Fields with STL files*



### Features

- Import STL files with a simple drag-and drop.
- Visualize STL files with eye-pleasing smooth shading
- Generate signed distance fields, and export [Paule Bourke](http://paulbourke.net/dataformats/volumetric/#:~:text=Volume%20data%20format&text=A%20bit%20like%20the%20PPM,as%20having%20more%20header%20fields.) volumetric data



### Examples

| ![](D:\Projects\SDFBuilder\ReadmeFiles\screenshot_knight.png) |
| :----------------------------------------------------------: |
| ![](D:\Projects\SDFBuilder\ReadmeFiles\screenshot_helios.png) |

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

- Select `Build>>Build Solution` in the Visual Studio toolbar

##### Converting and Visualizing Files

- Run the project by selecting `Debug>>Start without debugger` in the visual studio toolbar

- Drag and drop an STL file from any location into the application window

- Use the live visualization to track conversion progress

- Locate the output SDF file in the STL source folder (e.g. `desktop/Knight.stl`→`desktop.Knigh,sdf`)

  