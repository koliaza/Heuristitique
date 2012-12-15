This is an implementation in C of a heuristic algorithm for the graph isomorphism problem.

There is a makefile to compile.

The program is run with the following arguments : 
- --verbose to print all stages (advised)

- directory name to compare isomorphisms in all files of the directory (directory must only contain graph files)
or
- graph1 graph2 to check if there is an isomorphism among the graphs

The test_gen can be used as follows (all arguments are mandatory, or the help will be displayed) : 
test_gen n numgraphs (p || m || kreg) mode iso
-  n : number of vertices, 
- numgraphs : number graphs to generate 
- p and m : as in G(n,p) and G(n,m) 
- kreg : degree when kregular
- mode : 1 for G(n,p), 2 for G(n,m), 3 for G(n,m) multigraph,
         4 for k-regular, 5 for k-regular multigraph
- iso  : 1 for generating graphs isomorphic to the first generated

All files are released under WTF public license.
