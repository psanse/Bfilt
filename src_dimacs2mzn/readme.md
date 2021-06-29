**MiniZinc model generator**

The folder provides the conversion algorithm  from a dimacs graph to minizinc format, assuming that the graph format corresponds to a Binary Constraint Satisfaction Problem instance obtained as explained in the paper:

> A new branch-and-filter exact algorithm for binary constraint satisfaction problems  
by Pablo San Segundo, Fabio Furini and Rafael León. European Journal of Operational Research, (under review 2021).

The instructions are as follows:  

1.  Compile the file 'dimacs_to_minizinc_2.3.cpp' in the src folder (tested in Linux 16.04.7 and Windows 10- c++11 flag). Name the binary preferably 'dim_to_mnz' (see later).
2.  Execute the command line:


>dim\_to\_mnz \<filename in DIMACS format, extension *.clq>

The output is a \<filename.mzn> file.

We provide a demonstration example in the example folder.

To note:

*  The converter requires that the graphs have the extension (*.clq). It also requires the existence of the layer information (i.e. a declaration of the number of variables and domain size of the original BCSP instance) with the extenson (*.csp).

* The bin folder provides a Linux binary (Ubuntu) and a python script minizinc_generator.py (which only works for a 'dim\_to\_mnz' binary). Use the python script as follows:

>minizinc_generator.py \<folder>

and it will recursively produce minzinc models for all subdirectories.


