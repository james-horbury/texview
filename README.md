# TexView 
## The OpenGL Texture Viewer

This is an adaptation of a few of the introductory sections of [learnopengl.com](https://learnopengl.com/Getting-started/Coordinate-Systems), 
created by [Joey DeVries](https://github.com/JoeyDeVries). All libraries that require precompiled binaries (assimp & glfw) are recommended to be 
built locally to ensure compatability and stability across environments.

# How to Run

To compile simply execute the `make` command from the project directory. Use `make clean` to remove all compiled binaries and generated files.

# Theory

This project demonstrates a basic version of the Phong Lighting Model wherein a combination of the Phong Reflection BRDF and Lambertian Diffusion BRDF are 
utilized to approximate a solution to the Rendering Equation. In essence, it is an attempt at accurately modeling outgoing light given the following parameters:
- x = The position in space
- $\vec{w}$ = Outgoing light vector
- $\vec{w}$ = Incoming light vector
- $L_i$ = Light color
- $\vec{n}$ = Normal vector
- $\Theta$ = Incident angle
- $\vec{R}$ = Reflection vector

# Notes

An ImGui.ini file will be automatically created after running the program, this will preserve ImGui window information for subsequent instances but 
will be removed following the use of the `make clean` command.


![demo](assets/media/demo.gif)
