#ifndef SERIAL_MIS_H
#define SERIAL_MIS_H

#include "globals.h"
#include "graph.h"

void serial_mis(int* mis_array) {
    //printf("Start sorting \n");
    //double setup_start = tfk_get_time();
    qsort(orderedVertices, sparse_rep.numRows, sizeof(unsigned int), &compare);
    //printf("Done sorting \n");
    //double run_start = tfk_get_time();

    //printf("Time spent sorting: %f \n", (run_start - setup_start));

    for (int k = 0; k < sparse_rep.numRows; k++) {
      int vid = orderedVertices[sparse_rep.numRows - k - 1];
      //int vid = orderedVertices[k];
    
      //assert(mis_array[vid] != 1);
      if (mis_array[vid] == 0) {
        continue;
      }

      mis_array[vid] = 1;
      int* neighbors = &sparse_rep.ColIds[sparse_rep.Starts[vid]];
      unsigned int degree = (sparse_rep.Starts[vid + 1] - sparse_rep.Starts[vid]);
      for (int i = 0; i < degree; i++) {
        mis_array[neighbors[i]] = 0;
      }
    }
    //printf("Done run \n");
    //double run_end = tfk_get_time();

    //printf("Time spent run: %f \n", (run_end - run_start));
}
#endif  // SERIAL_MIS_H
