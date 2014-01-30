#ifndef ASYNC_MIS_H_
#define ASYNC_MIS_H_

#include "globals.h"
#include "tournament.cpp"

void compute_mis_tournament(int* mis_array, Vertex *__vertices, unsigned char *__tournamentArray) {
    unsigned int numEdges =
        (unsigned int) sparse_rep.Starts[sparse_rep.numRows];
    //Vertex *__vertices = (Vertex *) malloc(sizeof(Vertex)*sparse_rep.numRows);
    //unsigned char *__tournamentArray = (unsigned char *) malloc(numEdges);
    cilk_for( unsigned int i = 0; i < sparse_rep.numRows;
        i++ ) { // cilk_for
        __vertices[i].vertex_init(__vertices, __tournamentArray, i,
            (unsigned int *) sparse_rep.ColIds,
            sparse_rep.Starts[i],
            sparse_rep.Starts[i + 1] - sparse_rep.Starts[i]);
    }
    cilk_for( size_t i = 0; i < sparse_rep.numRows; ++i) {
        if (__vertices[i].numPredecessors == 0){
            int degree =
                sparse_rep.Starts[i + 1]
                - sparse_rep.Starts[i];
            __vertices[i].mark_vertex(
                __vertices,
                __tournamentArray,
                mis_array,
                (unsigned int *) sparse_rep.ColIds,
                1L);
        }
    }

    //  cilk_for( int i = 0; i < sparse_rep.numRows; i++ ) { // cilk_for
    //    if (__vertices[i].numPredecessors
    //        == 0) {
    //      int degree =
    //          sparse_rep.Starts[i + 1]
    //                            - sparse_rep.Starts[i];
    //      __vertices[i].color_vertex(
    //          __vertices,
    //          __tournamentArray,
    //          (unsigned int *) sparse_rep.ColIds,
    //          1L);
    //    }
    //  }
    // MERELY COPYING THE COLORS INTO THIS OTHER ARRAY TAKES ~10-15% of the time!!!
    // THIS ISN'T REALLY FAIR - PERHAPS WE SHOULD VALIDATE THE COLORING ON THE
    // __vertices[] ARRAY.
    //for( int i = 0; i < sparse_rep.numRows; i++ ) { // cilk_for
    //  vertexColors[i] = __vertices[i].finalColor;
    //}

    //free(__vertices);
    //free(__tournamentArray);
}


#endif /* ASYNC_MIS_H_ */
