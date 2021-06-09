**
# Dimacs graph generator

The folder provides the conversion algorithm from an XML file to a Dimacs graph format, assuming that the graph format corresponds to a Binary Constraint Satisfaction Problem instance obtained as explained in the paper:

> A new branch-and-filter exact algorithm for binary constraint satisfaction problems
by Pablo San Segundo, Fabio Furini and Rafael León. European Journal of Operational Research, (under review 2021).

The pre-requisites to compile this program are:

1. XML library, libxml2, is required to process XML files. The packets libxml2 and libxml2-dev have to be installed in the system.
2. XCSP3 C++ parser version, https://github.com/xcsp3team/XCSP3-CPP-Parser, is also used to process the XML files that are written in XCSP3 format.

The instructions are as follows:

1. Build the binary *xml2dimacs* from the *xml2dimacs.cc* source file in the *src* folder with CMake (CMakeLists.txt is provided, see specific instructions in the folder). 

2. Execute the command line:

> ./xml2dimacs <filename in XCSP3 format, extension *.xml>

The output are the files: <filename.clq> and <filename.csp>.

We provide a demonstration example in the example folder.

To note:

* The converter requires that the graphs have the extension (.xml). XCSP3 file collections are packed compressed using Lempel–Ziv–Markov chain algorithm (LZMA), they need to be uncompressed before. In the *bin* folder the script *uncompress_files.py* is provided to uncompress files recursively as follows:

> uncompress\_files.py \<folder>

* The bin folder also provides the python script, *graph\_gen.py* will recursively produce dimacs graphs for all subdirectories. It currently requires a binary *xml2dimacs*. Use the python script as follows:

> graph\_gen.py \<folder>


