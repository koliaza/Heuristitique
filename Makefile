heuristitique: main.c util.h util.c graph.h graph.c io.h io.c matrix_mod.h matrix_mod.c sorting.h sorting.c partitions.h partitions.c pn_heuristic.h pn_heuristic.c
	gcc -Wall -g -D_REENTRANT util.c graph.c io.c matrix_mod.c sorting.c partitions.c pn_heuristic.c main.c -lm -lrt -lpthread -o heuristitique

test_gen: test_gen.c util.h util.c graph.h graph.c random_graph.h random_graph.c io.h io.c
	gcc -Wall -g util.c graph.c random_graph.c io.c test_gen.c -lm -o test_gen
