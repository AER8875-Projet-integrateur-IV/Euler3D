## External Dependancies
[OpenMPI](https://www.open-mpi.org/)

[METIS](http://glaros.dtc.umn.edu/gkhome/metis/metis/overview)

### Adding a boost dependancy

1. Check if the library is Header only

1. Download the [boost copy tool](https://www.boost.org/doc/libs/1_75_0/tools/bcp/doc/html/index.html)

2. Add you dependancy to ./extern/boost using the following command
    ```
    $ bcp --path=/path/to/boost/dir scoped_ptr /extern
    ```
    Where scoped_ptr is the header you want to add

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



