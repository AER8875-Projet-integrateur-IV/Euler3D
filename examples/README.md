# Build process

If it has not been done already, build the program by following the instructions in [INTALL.md](../INSTALL.md)

# Running examples

make sure the following executable are in this directory before running the examples

* E3D_PRE
* E3D_POST
* E3D_Solver

You can use the following commands to copy the executables to this folder after
compilation

```
    $   cp ../build/src/E3D_PRE E3D_PRE
    $   cp ../build/src/E3D_Solver E3D_Solver
    $   cp ../build/src/E3D_POST E3D_POST
```

# Available examples

## Empty domain 

This simulation should converge in a single iteration. It is a simple test to check that the program has been correctly generated.

Run instructions:
```
    $   cd empty_domain
    $   ./empty_domain.bsh
```

## naca0012 9x9x5 subsonic

Mach = 0.5

AOA= 1.25

Run instructions:
```
    $   cd naca0012_9x9x5_subsonic
    $   ./naca0012_9x9x5_subsonic.bsh
```

## naca0012 9x9x5 transsonic

Mach = 0.8

AOA= 1.25

Run instructions:
```
    $   cd naca0012_9x9x5_transsonic
    $   ./naca0012_9x9x5_transsonic.bsh
```

## naca0012 9x9x5 supersonic

Mach = 1.25

AOA= 1.25

Run instructions:
```
    $   cd naca0012_9x9x5_supersonic
    $   ./naca0012_9x9x5_supersonic.bsh
```

## ONERA M6 transsonic

Mach = 0.8

AOA= 1.25

Run instructions:
```
    $   cd ONERA_transsonic
    $   ./ONERA_transsonic.bsh
```
