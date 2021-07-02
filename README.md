TJN Test
--------

WARNING: THIS IS ONLY A TEST TO HAVE CMAKE/C++ HELLO WORLD MOST SHOULD
IGNORE THIS AND LOOK AT THE ORIGINAL.

This is a CMake Hello World (C++) with additions of the
Alltoall spc timer example code to test using it to access
the non-public C helper function that lives in our libmpi.so

 - 1. Load modules (e.g., `cmake`, `openmpi/onlytimer.debug`)
 - 2. Make build subdir
    ```
       cd cmake-hello-world/
       mkdir build-mpi/
    ```

 - 3. Change to build subdir and build
    ```
       cd build-mpi/
       cmake .. && make
    ```

 - 4. Run example w/ extern ref to helper function
    ```
       mpirun -np 4 ./CMakeHelloWorld
    ```

====

This is an extremely simple example C++ application which uses CMake. 
It will build and install an application called CMakeHelloWorld which simply outputs "Hello, world!" to stdout.  

### Installation ###
As this is an example of how to use CMake you'll need to download it (http://www.cmake.org/cmake/resources/software.html) or install it via:
```bash
sudo apt-get install cmake
```
Once CMake has been install navigate to the root of the project and issue the following commands:
```bash
mkdir build
cd build
cmake .. && make
make install
```
You may need to use`sudo make install` command as it will install to /usr/local/bin .  This might be a good opportunity to investigate stow (http://www.gnu.org/software/stow/manual/stow.html) as a means of managing the installation of applications in this manner, however I haven't gotten far enough into my CMake knowledge to take that step.

### Usage ###
Once the executable has been created simply call `CMakeHelloWorld`, either from the build directory or anywhere (if you installed to /usr/local/bin, etc).

***Uninstallation*** 
Removing the application is straight-forward.  Simply delete the binary 'CMakeHelloWorld' from /usr/local/bin.
   

***Caution***
I am a complete C++ noob.  I've created this project as part of an effort to kickstart my C++ recovery as I have recently been placed on a team which is developing an embedded application.  There may be any number of things wrong with this example.  If you see something you know to be implemented incorrectly please contact me via github and I will do my best to correct it.  Better, fork the project, fix it and issue a pull request.
