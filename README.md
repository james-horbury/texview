# TexView: The OpenGL Texture Viewer

This project began as an adaptation of a few of the exercises in the introductory sections of [learnopengl.com](https://learnopengl.com/Getting-started/Coordinate-Systems),
by [Joey DeVries](https://github.com/JoeyDeVries). It evolved into an attempt at creating a simple texture viewer allowing a technical artist to preview textures and
experiment with how they might appear when rendered using different lighting and material properties. Specifically, this program allows the user to observe a voxel, with 
a default Minecraft texture, under the effects of the Phong Lighting Model.

All libraries that require precompiled binaries, such as assimp and glfw, are recommended to be built locally to ensure compatability and stability across environments.

## How to Run

To compile, simply execute the `make` command from the project directory. Use `make clean` to remove all compiled binaries and generated files.

## The Theory

This project demonstrates a basic version of the Phong Lighting Model wherein a combination of the Phong Reflection BRDF and Lambertian Diffusion BRDF are 
utilized to approximate a solution to the Rendering Equation. However, as this is an attempt at real-time rendering, the Rendering Equation used has been simplified 
considerably in order for computation to stay performant. The most notable change may be the omission of the emissive light component and the removal of the integral in 
front of the BRDF. [why is this?]

The Rendering Equation:

$L_o(\mathbf{x}, \vect{\omega}_o) = L_e(\mathbf{x}, \vect{\omega}_o) + \int_{\Omega} f_r(\mathbf{x}, \vecet{\omega}_i, \vect{\omega}_o) L_i(\mathbf{x}, \vect{\omega}_i) (\vect{\omega}_i \cdot \mathbf{n}) \, d\vect{\omega}_i$

Reduced Form (what we use in our model):

$L_o(\mathbf{x}, \vect{\omega}_o) = (f_diffuse(\mathbf{x}, \vecet{\omega}_i, \vect{\omega}_o) + f_specular(\mathbf{x}, \vecet{\omega}_i, \vect{\omega}_o)L_i\cos(\theta)_i$

where,

$f_diffuse(\mathbf{x}, \vecet{\omega}_i, \vect{\omega}_o) = C_diffuse$

$f_specular(\mathbf{x}, \vecet{\omega}_i, \vect{\omega}_o)$

x = The position in space
$\vec{w}_o$ = Outgoing light vector
$\vec{w}_i$ = Incoming light vector
$L_i$ = Incoming light color
$\vec{n}$ = Normal vector
$\Theta$ = Incident angle
$\vec{R}$ = Reflection vector

As this is an attempt at real-time rendering, the Rendering Equation used has been simplified considerably in order for computation to stay performant. The most 
notable change may be the omission of the emissive light component and the removal of the integral in front of the Bidirectional Reflectance Distribution Function 
(BRDF). 

[show full rendering equation and simplified version used in the program]

## Notes

An ImGui.ini file will be automatically created after first running the program. This will preserve ImGui window information for subsequent instances but will be 
removed following the use of the `make clean` command.


![demo](assets/media/demo.gif)
