This folder contains a linux binary release for the algorithm Bfilt.

Date of release@09/10/2021
Compiled with Ubuntu 16.04.7 LTS

%%%%%%%%%%%%
%%
%% INSTRUCTIONS
%%
%%%%%%%%%%%

Run the binary with the name of the graph (in DIMACS format, <filename>.clq) and time limit (in seconds).

Please note that the corresponding  <filename>.csp file with the layer information must be in the same folder.

Example:

./Bfilt <filename> 1000   will time out in 1000 seconds.

%%%%%%%%%%%%%%%%%%
%%
%% TOY EXAMPLE
%%
%%%%%%%%%%%%%%%%%%


The folder includes the files toy_paper.clq and toy_paper.csp (and some more instances from the RB2 family) that encode the reduction of a BCSP toy example to a k-CLP. The example is described in Figure 1 of the reference paper (see main page), and contains 4 variables with domains x1={1, 3, 5}, x2={1, 2, 3}, x3={5, 6} and x4={1, 2, 3} respecively.

The toy.csp file is therefore:
x 4
v 1 3
v 2 3
v 3 2
v 4 3

The first line x <NUM> indicates the number of layers, also the number of variables in the BCSP (in this case 4).
The remaining lines, v <x> <y> indicate the size of the domains for each variable (layer in the graph), i.e.,  v 1 3 reads that the first variable (x1 in the example) has 3 values.

The DIMACS graph (toy-paper.clq) indicates the compatibility between two (variable, value) pairs.
The first few lines are:

c toy paper dimacs
p edge 11 15 
e 1 4
e 1 5
e 1 7
e 1 9
...

For example, the first edge (1, 4) indicates that the first value of x1 is compatible with the first value of x2 (the vertices are ordered according to variable (layer) index and domain order). Vertex 4 refers to the first value in x2, since x1 has 3 values represented by the first 3 vertices in the graph.


Running:


./Bfilt <path>toy-paper.clq 1000   

ouptuts the following information in the console:

	  read: C:\Users\pablo\Desktop\CSP_benchmarking\toy_paper\toy_paper.clq    n:11    m:15
	  SATISFIABLE
	  n:11 m:15 omega:4 + fixed:0 t_parse: 0.01 t_preproc: 0.052 t_search: 0
	  ----------------------------
	  Each number corresponds to a variable-value pair in the original BCSP / layer-value pair in the graph-

	  0 3 6 8  [4] 

The BCSP is SATISFIABLE, omega:4 is the clique number of the microstructure graph, no values were fixed during preprocessing,  t_parse is the parsing time, t_preproc is the preprocessing time and t_search is the time spent on the NP-complete search procedure.
Finally, the solution:

0 3 6 8  [4]

indicates the vertices that form the 4-clique, each one associated to a layer (variable in the BCSP) in increasing order. From the csp.file, given the order of each variable domain:
x1 / layer 1: [0]-first value...x1[1]
x2 / layer 2: [3]-first value...x2[1]
x3 / layer 3: [6]-first value...x3[5]
x4 / layer 4: [8]-first value...x4[1]

The actual values of the variables (in brackets, at the end of each line) are not available directly from the graph encoding, and have to be taken from the original BCSP instance in XCSP3 format.




   