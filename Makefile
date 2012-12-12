heuristitique: main.c util.h util.c graph.h graph.c random_graph.h random_graph.c io.h io.c pn-heuristic.h pn-heuristic.c
	gcc -Wall util.c graph.c random_graph.c io.c pn-heuristic.c main.c -lm -o heuristitique
