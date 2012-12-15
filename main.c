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
    dir = opendir(directory_path);
    if (dir != NULL) {
        while ((entry = readdir (dir)) != NULL) {
            if (!(strcmp(entry->d_name, ".") == 0) &&
                !(strcmp(entry->d_name, "..") == 0)) {
                
            }
        }
        closedir (dir);
    } 
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
