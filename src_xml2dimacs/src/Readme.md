
# Source folder


This folder contains the source code xml2dimacs.cc for the translator from BCSP instances in XCSP3 format to K-CP instances in DIMACS format.

Compilation requires CMake. The CMakeLists.txt configuration file assumes that the source program is located in a directory called *xml2dimacs* at the route of the XCSP3 C++ parser folder.

It is advisable to create a build directory and execute the cmake command from this directory.

**IMPORTANT**: A copy of the file *XCSP3PrintCallbacks.h* has to be on the *xml2dimacs* directory.
