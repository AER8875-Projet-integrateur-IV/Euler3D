if [ -f "openmpi/bin/mpirun" ] && [ -f "openmpi-4.0.1/config.log" ]; then
	echo "Using cached OpenMPI"
	echo "Configuring OpenMPI"
	cd openmpi-4.0.1
	./configure --prefix=$TRAVIS_BUILD_DIR/openmpi CC=$C_COMPILER CXX=$CXX_COMPILER &> openmpi.configure
    else
        # install OpenMPI from source
	echo "Downloading OpenMPI Source"
	wget https://download.open-mpi.org/release/open-mpi/v4.0/openmpi-4.0.1.tar.gz
	tar zxf openmpi-4.0.1.tar.gz
	echo "Configuring and building OpenMPI"
	cd openmpi-4.0.1
	./configure --prefix=$TRAVIS_BUILD_DIR/openmpi CC=$C_COMPILER CXX=$CXX_COMPILER &> openmpi.configure
	make -j4 &> openmpi.make
	make install &> openmpi.install
	cd ..
fi

test -n $CC && unset CC
test -n $CXX && unset CXX