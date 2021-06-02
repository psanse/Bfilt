A binary constraint satisfaction problem (BCSP) consists in determining an assignment of
values to variables that is compatible with a set of constraints. A BCSP is a Combinatorial Constraint Programming problem. XCSP3 is an XML format used to define Combinatorial Constraint Problems instances. Large number of problems families are defined using it.

An XML parser has been written in C++ and it is located at:

https://github.com/xcsp3team/XCSP3-CPP-Parser

It provides a collection of callback methods that helps processing benchmark files. The software xml2dimacs transforms XCSP3 files into a graph, it implements a set of callback functions that they are invoked after the corresponding tag is found in the file. 
