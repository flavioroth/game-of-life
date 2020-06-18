##### Projet goal

Make a simple game of life "player" that runs on Linux.


##### Step 1 - Design choices

I would like to explore the Hashlife algorithm and I'm going that start by implementing it on the CPU.
I would also like the implementation to be fast, so I'm going to try to spend as little time as possible drawing the pixels by using the GPU via OpenGL for that.
I also need a basic UI for the user to interact with the simulation (zoom, pause etc)

I'm going to use C++ as language, ImGUI + GLFW for the graphical context and OpenGL as drawing API. CMake as the build system.

##### Step 2 - Basic app that displays a blue screen

Let's make a c++ app that :

* Creates a window
* Initializes the graphical context
* Fill the screen with blue
* Wait for the user to request exit
* Tear down the graphical context on exit

