#!/bin/sh

if [ -f mpich/lib/libmpich.so ]; then
  echo "libmpich.so found -- nothing to build."
else
  echo "Downloading mpich source."
  wget http://www.mpich.org/static/downloads/3.4.1/mpich-3.4.1.tar.gz
  tar xfz mpich-3.4.1.tar.gz
  rm mpich-3.4.1.tar.gz
  echo "configuring and building mpich."
  cd mpich-3.4.1
  ./configure \
          --prefix=`pwd`/../mpich \
          --enable-static=false \
          --enable-alloca=true \
          --disable-long-double \
          --enable-threads=single \
          --enable-fortran=no \
          --enable-fast=all \
          --enable-g=none \
          --enable-timing=none \
          --with-device=ch3
  make -j4
  make install
  cd -
  rm -rf mpich-3.4.1
fi