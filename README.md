# ChillerBot User Xp

Made for main client usage. Based on DDNet which is based on DDrace which is based on Teeworlds.

Cloning
-------

To clone this repository with full history and external libraries (~350 MB):

    git clone --recursive https://github.com/chillerbot/chillerbot-ux

To clone the libraries if you have previously cloned chillerbot without them:

    git submodule update --init --recursive

Building on Linux and macOS
---------------------------

To compile DDNet yourself, execute the following commands in the source root:

    mkdir build
    cd build
    cmake ..
    make


