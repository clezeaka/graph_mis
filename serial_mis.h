#ifndef SERIAL_MIS_H
#define SERIAL_MIS_H

#include "globals.h"
#include "graph.h"

void serial_mis(int* mis_array) {
  for (int k = 0; k < sparse_rep.numRows; k++) {
    int vid = orderedVertices[i];

    assert(mis_array[vid] != 1);
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
}
#endif  // SERIAL_MIS_H
