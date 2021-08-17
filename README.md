# 2D Computer Graphics Engine
Course: COMP 475 at UNC Chapel Hill

Semester: Fall 2020

Professor: Mike Reed

 - A graphics library built in C++ over the course of six programming assignments and a (timed, three hour) final exam.
 - Its fundamental purpose is to manipulate a pixel matrix according to the programmer/artist/user’s specification.
 - Core functionality includes: drawing of geometric primitives, blending and alpha compositing, clipping, fragment/pixel shaders (linear/radial gradients, bitmap/texture sampling), shader tiling, line stroking, matrix algebra and linear transformations of graphics objects, concave shapes via path drawing, Bezier curves (quads, cubics), and triangle meshes.
 - The project is an exercise in scan conversion, and I learned a great deal by building the aforementioned features from the ground up.
 - It is worth noting that the instructor provided the interface through which the engine interacted with the 2D pixel array, as well as many structural headers that gave direction as to how functionality should be implemented.
 - Extensive use of the C++ debugger, GDB, was required to create a functioning engine, as well as a C++ profiler, which was used to optimize certain processes.
 - All of the following images listed as 'Artwork' (except for the results of the final exam output grader) are of my own creation

## Assignment 1
| Description | Artwork |
| - | - |
|<ul><li>Implemented a canvas class based off of an interface provided by Professor Reed</li><li>The clear() method fills the entire canvas with a specified color, while the fillRect() method fills affected pixels within a rectangular region with a specified color</li><li>Blending was introduced in this assignment, as the clear() method used source blending and the fillRect() method used source-over</li><li>Rudimentary clipping was also covered, as parts of the rectangle outside of the canvas were ignored</li></ul> | ![A1 artwork](readme_assets/pa1.png?raw=true) |

## Assignment 2
| Description | Artwork |
| - | - |
|<ul><li>Extended the canvas class to draw convex polygons with various blend modes</li><li>Support for Professor-defined blend modes were added to all drawing functions</li><li>The drawConvexPolygon() method fills an area specified by a list of points with a specified paint (color & blend mode)</li><li>Clipping explored in greater detail in this assignment</li> | ![A2 artwork](readme_assets/pa2.png?raw=true) |

## Assignment 3
| Description | Artwork |
| - | - |
 |<ul><li>Determined pixel color based on shaders and added linear transformations with matrices</li><li>Implemented matrix methods for translation, rotation, scale, multiplication</li><li>Added a Bitmap shader for texture sampling</li> | ![A3 artwork](readme_assets/pa3.png?raw=true) |

## Assignment 4
| Description | Artwork |
| - | - |
 |<ul><li>Added non-convex polgyons through edge paths and implemented a Linear Gradient shader</li><li>Arbitrary shape drawing through a path class, which required winding functionality</li> | ![A4 artwork](readme_assets/pa4.png?raw=true) |

## Assignment 5
| Description | Artwork |
| - | - |
 |<ul><li>Added support for bezier curves and multiple tile modes for shaders</li><li>Quadratic and cubic bezier curves were added to the path class, which also allowed for circle drawing</li><li>Clamp, repeat, and mirror tile mode support was added to all shaders</li> | ![A5 artwork](readme_assets/pa5.png?raw=true) |

## Assignment 6
| Description | Artwork |
| - | - |
 |<ul><li>Triangle meshes introduced to the engine</li><li>Given a list of vertices (containing position, color, texture coordinates), a list of indices, and a shader, a mesh is drawn</li><li>Tesselation of quads also implemented</li> | ![A6 artwork](readme_assets/pa6.png?raw=true) |

## Final Exam
| Description | Artwork |
| - | - |
|<ul><li>Task was to implement at least a Radial Gradient shader or Stroked Line contour for the path class</li><li>I chose to implement both</li><li>Radial Gradient is defined by a center, radius, and list of colors</li><li>Stroke is defined by two points, a width, and a cap type (butt, square, round)</li> | <div>![Final Exam Radial Shader demonstration](readme_assets/final_radial.png?raw=true)</div><div>![Final Exam Stroke Drawing demonstration](readme_assets/final_stroke.png?raw=true)</div> |
