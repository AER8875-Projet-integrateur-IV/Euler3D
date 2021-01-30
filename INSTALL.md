## Dependancies
[OpenMPI](https://www.open-mpi.org/)

[METIS](http://glaros.dtc.umn.edu/gkhome/metis/metis/overview)

## Build process

1. Create a build directory
```   
$ mkdir build
$ cd build
```

2. Run CMake

Use the command `cmake [options] ..`

Relevant options are:

`-DCMAKE_BUILD_TYPE=Debug` Build with debug flags

`-DCMAKE_BUILD_TYPE=Release` Build with Release flags

`-DPACKAGE_TESTS=ON` Build tests

Minimal example: ` $ cmake ..`

***Note: you have to delete your build directory and start from scratch if you
want to use new options***

3. Compile 
```   
$ cmake --build .
```

## Running tests
If you chose to build the tests, you can run them with the following command 
from the build folder `./test/tests`



