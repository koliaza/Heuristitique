#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include "io.h"
#include "pn_heuristic.h"

void test_2_graphs(char *file1, char *file2) {
    graph_list *g1 = NULL;
    graph_list *g2 = NULL;

    FILE *f;
    
    f = fopen(file1, "r");
    g1 = fget_graph_list(f, NULL);
    fclose(f);

    f = fopen(file2, "r");
    g2 = fget_graph_list(f, NULL);
    fclose(f);

    int *isom = malloc(g1->n * sizeof(int));
    int i;

    if (find_isomorphism(g1, g2, isom)) {
        printf("Here is an isomorphism between the 2 graphs:\n");
        for (i = 0; i < g1->n; i++) {
            printf("%d -> %d\n", i, isom[i]);
        }
    } else {
        printf("The 2 graphs are not isomorphic.\n");
    }
    
    if (g1 != NULL) gl_free(g1);
    if (g2 != NULL) gl_free(g2);
    free(isom);
}

void test_multiple_graphs(char *directory_path) {
    DIR *dir;
    struct dirent *entry;
    int array_size = 16;
    graph_list **graphs = malloc(array_size*sizeof(void*));
    int graph_count = 0;
    int *isom_classes;
    FILE *f;

    dir = opendir(directory_path);
    if (dir == NULL) exit(1);
    while ((entry = readdir (dir)) != NULL) {
        if (!(strcmp(entry->d_name, ".") == 0) &&
            !(strcmp(entry->d_name, "..") == 0)) {
            graph_count++;
            if (graph_count > array_size) {
                graph_list **tmp = malloc(2*array_size*sizeof(void*));
                memcpy(tmp, graphs, array_size*sizeof(void*));
                free(graphs);
                array_size *= 2;
                graphs = tmp;
            }
            f = fopen(entry->d_name, "r");
            graphs[graph_count - 1] = fget_graph_list(f, NULL);
            fclose(f);
        }
    }

    isom_classes = malloc(graph_count * sizeof(int));
    find_multiple_isomorphisms(graph_count, graphs, isom_classes);

    closedir (dir);
    int i;
    for (i = 0; i < graph_count; i++)
        gl_free(graphs[i]);
    free(graphs);
}

void usage() {
    fprintf(stderr, "Usage: heuristitique file1 file2\n");
    fprintf(stderr, "       heuristitique directory\n");
    exit(0);
}

int verbose = 0;

int main(int argc, char *argv[]) {
    int status;
    struct stat stat_buffer;

    if (argc == 1) usage();

    if (strcmp(argv[1], "--verbose") == 0) {
        if (argc == 2) usage();
        verbose = 1;
        argv++; //small hack to make it easier
        argc--;
    }

    status = stat(argv[1], &stat_buffer);
    if (status != 0) {
        fprintf(stderr, "%s : no such file or directory\n", argv[1]);
        return 1;
    }
    if (S_ISDIR(stat_buffer.st_mode))
        test_multiple_graphs(argv[1]);

    if (S_ISREG(stat_buffer.st_mode)) {
        if (argc < 3) usage();
        if (!( (stat(argv[2], &stat_buffer) == 0)
               && (S_ISREG(stat_buffer.st_mode)))) {
            fprintf(stderr, "%s : no such file\n", argv[2]);
            return 1;
        }
        test_2_graphs(argv[1], argv[2]);
    }

    return 0;
}
