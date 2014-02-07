#include <assert.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <cmath>
#include <string>

#include "graphUtils.h"
#include "ordering.h"
#include "graphIO.h"
#include "serial_mis.h"
#include "async_mis.h"


void validate_mis(int* mis_array) {
  for (int i = 0; i < sparse_rep.numRows; i++) {
    assert(mis_array[i] != -1);
  }
  
  //printf("%d\n", sparse_rep.numRows);
  for (int k = 0; k < sparse_rep.numRows; k++) {
    //printf("%d\n", k);
    
    int* neighbors = &sparse_rep.ColIds[sparse_rep.Starts[k]];
    unsigned int degree = (sparse_rep.Starts[k + 1] - sparse_rep.Starts[k]);
    if (mis_array[k]) {
      assert(mis_array[k] == 1);
      for (int i = 0; i < degree; i++) {
        //printf("%d\n", neighbors[i]);
        assert(mis_array[neighbors[i]] == 0);
      }
    } else {
      int has_neighbor_in_mis = 0;
      for (int i = 0; i < degree; i++) {
        if (mis_array[neighbors[i]] == 1) {
          has_neighbor_in_mis = 1;
          continue;
        }
      }
      assert(has_neighbor_in_mis);
    }
  }
}

int main(int argc, char **argv)
{
  if (argc != 3) {
    printf("Invalid args. Usage: ./main <file> <algorithm {serial, tournament}>\n");
  } else {
    string algorithm = argv[2];
    
    if (!(algorithm.compare("serial") == 0 || algorithm.compare("tournament") == 0)) {
      printf("Invalid argument for algorithm. Should be serial or tournament\n");
      return 0;
    }
    
    printf("Loading input graph...\n");
    string inputname = argv[1];
    //sparseRowMajor<int,int> sparse_rep;
    // Parse the graph in .mtx or adjacency list representation.
    if (inputname.find(".mtx") != string::npos) {
      graph<int> inter = graphFromEdges<int>(edgesFromMtxFile<int>(inputname.c_str()), true);
      sparse_rep = sparseFromGraph<int>(inter);
    } else if (inputname.find(".edges") != string::npos) {
      edgeArray<intT> eArr = readEdgeArrayFromFile<int>((char*)inputname.c_str());
      graph<int> inter = graphFromEdges<int>(eArr, true);
      sparse_rep = sparseFromGraph<int>(inter);
    } else {
      graph<int> inter = graphFromEdges<int>(edgesFromGraph<int>(readGraphFromFile<int>((char*)inputname.c_str())), true);
      sparse_rep = sparseFromGraph<int>(inter);
    } 
    
    printf("Initializing mis_array...\n");
 
    int* mis_array = (int*) calloc(sparse_rep.numRows, sizeof(int));
    for (int i = 0; i < sparse_rep.numRows; i++) {
      mis_array[i] = -1;
    }
    
    printf("Running mis algorithm...\n");
    double color_start = tfk_get_time();
    
    if (algorithm.compare("serial") == 0){
      serial_mis(mis_array);
    } else if (algorithm.compare("tournament") == 0) {
      unsigned int numEdges = (unsigned int) sparse_rep.Starts[sparse_rep.numRows];
      Vertex* __vertices = (Vertex *) malloc(sizeof(Vertex)*sparse_rep.numRows);
      unsigned char *__tournamentArray = (unsigned char *) malloc(numEdges);
      compute_mis_tournament(mis_array, __vertices, __tournamentArray);
    } else {
      printf("Invalid argument for algorithm. Should be serial or tournament");
    }
    
    double color_end = tfk_get_time();
    printf("Time spent: %f \n", (color_end - color_start));
    
    validate_mis(mis_array);
  }

  return 0;
}

        

