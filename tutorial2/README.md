# Tutorial 2: OpenCV and Implementing the Sobel Filter

Justin Nguyen 09/30/2019

This tutorial assumes basic knowledge of C++, Makefiles, and Linux.

![Hello, Sobel!](media/sobel.png)

_Hello, Sobel!_

1. On most distributions of Linux, OpenCV needs to be built from source. Run the following commands from the terminal. This tutorial is based off of tutorials you can find online for building OpenCV on Ubuntu 18.04.

Upgrade your machine. Update repository to install latest dependencies.

```
$ sudo apt-get update -y
$ sudo apt-get upgrade -y
```

Install the dependencies for OpenCV.

```
$ sudo apt-get install build-essential 
$ sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev \
  libswscale-dev cmake-curses-gui
$ sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev \
  libtiff-dev libjasper-dev libdc1394-22-dev
```

Pull the latest release of OpenCV (currently 4.1.1).
 
```
$ wget https://github.com/opencv/opencv/archive/4.1.1.zip
```

Create the build directory and configuring OpenCV.

```
$ unzip 4.1.1.zip
$ cd opencv-4.1.1
$ mkdir build
$ cd build
$ ccmake ..
```

You should now be prompted with a nice GUI on configuring OpenCV. For now we will stick with the defaults. Press 'c' and wait for all the OpenCV build options to appear. Quit by pressing 'q'. This will save the configuration. Open ccmake again with the `$ ccmake ..` command and press 'g' to generate the build scripts (Makefile). After it has finished, press 'q' again to quit.

Building OpenCV. If you are using a VM, I would reccomend allocating more than 2GiB of memory and half of your computer's cores to the VM before running the command below. The make process will likely take 30+ minutes to build so go for a nice walk.

```
$ make -j$(nproc)
```

After OpenCV has built, install it.

```
$ sudo make install
```

_You now have OpenCV installed._

2. Create a new folder for your assignment.

```
$ cd
$ mkdir <assignment_name>
```

3. Create two files. A `main.cpp` file for your program and a `Makefile`. You can create your own Makefile or use my template from [Tutorial 1](https://github.com/justinnuwin/CPE442/blob/master/tutorial1/Makefile).

4. Using your text editor of choice open the `Makefile`. Populate the fields using the values below:

- CC: `g++`
- PROGRAM_NAME: `<your_choice>`
- CFLAGS: `-Wall -Werror -g`
- LDFLAGS: `-lopencv_core -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lm`
- C_HEADERS: `<add header files here if you create more>`
- C_SOURCES: `main..cpp <add more source files here if you create more>`

_The LDFLAGS field is for the linker to link external libraries into your program. For many libraries just including the include file is not enough as the include file does *not* contain the actual source of the library. For this program we are linking_ `opencv_core`, `opencv_highgui`, `opencv_videoio`, `opencv_imgcodecs`, _and_ `m`.

The OpenCV core library contains the neccessary OpenCV functions and datastructures. The OpenCV highgui library includes code for creating GUIs, displaying images, and drawing. The OpenCV videoio library has the neccessary code for opening videos through the VideoCapture interface. This interface supports many video formats in addition to webcams. The OpenCV imgcodecs library allows your program to open and save images of many types. The m library is the C standard math library.

5. 

6. Create a Make rule called `clean`. This rule does not require any prerequisites. The body of this rule (and any other Make rules) can execute arbitrary bash commands. This rule will delete a file called `PROGRAM_NAME`. The linux command for removing commands is `$ rm`.

7. Now that your Makefile template is complete for making a single target, fill in the variable names with the correct names. CC will usually be `gcc` or `clang`. Reccomended CFLAGS are `-Wall -Werror -g`. 

8. Run `$ make` from the command line and see if your program made successfully!

9. For the future, you can create additional Make rules andd variables to allow you to make multiple targets.
