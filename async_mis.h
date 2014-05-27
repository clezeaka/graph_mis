#ifndef ASYNC_MIS_H_
#define ASYNC_MIS_H_

#include "globals.h"
#include "tournament.cpp"

void compute_mis_tournament(int* mis_array, Vertex *__vertices, unsigned char *__tournamentArray) {
    unsigned int numEdges = (unsigned int) sparse_rep.Starts[sparse_rep.numRows];
    //Vertex *__vertices = (Vertex *) malloc(sizeof(Vertex)*sparse_rep.numRows);
    //unsigned char *__tournamentArray = (unsigned char *) malloc(numEdges);
    
    //printf("Start setup \n");
    //double setup_start = tfk_get_time();
    cilk_for( unsigned int i = 0; i < sparse_rep.numRows; i++ ) {
        __vertices[i].vertex_init(__vertices, __tournamentArray, i,
            (unsigned int *) sparse_rep.ColIds,
            sparse_rep.Starts[i],
            sparse_rep.Starts[i + 1] - sparse_rep.Starts[i]);
    }
    //printf("Done setup \n");
    //double run_start = tfk_get_time();

    //printf("Time spent setup: %f \n", (run_start - setup_start));

    cilk_for( size_t i = 0; i < sparse_rep.numRows; ++i) {
        if (__vertices[i].numPredecessors == 0){
            //printf("me");
            int degree =
                sparse_rep.Starts[i + 1]
                - sparse_rep.Starts[i];
            __vertices[i].mark_vertex(
                __vertices,
                __tournamentArray,
                mis_array,
                (unsigned int *) sparse_rep.ColIds,
                1);
        }
    }

    //printf("Done run \n");
    //double run_end = tfk_get_time();

    //printf("Time spent run: %f \n", (run_end - run_start));
}


#endif /* ASYNC_MIS_H_ */
