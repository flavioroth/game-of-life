##### Projet goal

Make a simple game of life "player" that runs on Linux.


##### Step 1 - Design choices

I would like to explore the Hashlife algorithm and I'm going that start by implementing it on the CPU.
Ideally, I would also like the implementation to be fast, so I'm going to try to spend as little time as possible drawing the pixels by using the GPU via OpenGL for that.
On top of that, it also needs a basic UI for the user to interact with the simulation (zoom, pause etc)

I'm going to use C++ as language, ImGUI + GLFW for the graphical context and OpenGL as drawing API. CMake as the build system.

##### Step 2 - Basic app that displays a blue screen

Let's make a c++ app that :

* Creates a window
* Initializes the graphical context
* Fill the screen with blue
* Wait for the user to request exit
* Tear down the graphical context on exit

##### Step 3 - Basic display

The app should be capable of displaying an array made of zeroes or ones on the GPU.

* Let's use unsigned bytes as it is the smallest addressable memory unit.
* We'll store the data on the GPU as a mono-channel red texture and make sure there's no interpolation when sampling it.
* The values will be remapped from [0, 1] to [0, 255] inside the fragment shader to be displayed as either white or black.


##### Step 4 - Game of life

Lets keep things simple and start by implementing a simple single-threaded GOL algorithm.

* The algo takes as input a reference to the (read-only) array holding the current generation and a reference to a (writable) array holding the next generation.
* Both matrices have the same dimensions
* For each cell coordinate set the corresponding cell state in the next generation matrix to the result of the GOL rule.
* Copy the next generation matrix to the GPU memory.
* Render on a quad.
