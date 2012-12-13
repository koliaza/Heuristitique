heuristitique: main.c util.h util.c graph.h graph.c random_graph.h random_graph.c io.h io.c matrix_mod.h matrix_mod.c pn_heuristic.h pn_heuristic.c
	gcc -Wall -g util.c graph.c random_graph.c io.c matrix_mod.c pn_heuristic.c main.c -lm -o heuristitique
