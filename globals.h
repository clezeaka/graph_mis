#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdint.h>

// Simple timer for benchmarks
double tfk_get_time()
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec*1e-6;
}

#define SERIAL_BIT_VECTOR true
#define COARSEN_FACTOR 512 // No longer used.
#define ORDER(i) order[i]
#define _DEBUG_ASSERT
enum ordering_method_t{
	RANDOM, DEGREE, LG_DEGREE, RANDOM_IT, INPUT
};
ordering_method_t async_order_method;

inline unsigned int compute_log(unsigned int v) {
unsigned int r = 0; // r will be lg(v)
while (v >>= 1) // unroll for more speed...
{
  r++;
}
 return r;
}

#define OVERFLOW_BIT_COLOR (1L << 63L)
#define MAX_BIT_COLOR 62
#define ALL_ONES 0xFFFFFFFFFFFFFFFF

inline int find_first_bit_set(unsigned long _color) {
	return 63 - __builtin_clzl(_color);
}

inline int find_open_color_bit(unsigned long _color){
	return find_first_bit_set(_color ^ (_color + 1));
}

sparseRowMajor<int,int> sparse_rep;
int orderedVertices [];

class Vertex
{
 public:
  unsigned int vertexID;
  unsigned int hashValue;
  unsigned int numPredecessors;
  unsigned int numSuccessors;
  volatile unsigned int finalInMIS;
  unsigned int edgeIndex;
  volatile uint64_t counter;
  unsigned char logSize;
  volatile unsigned char mutex;


  void vertex_init(Vertex *_vertices, unsigned char *_tournamentArray, unsigned int _vertexID,
		  unsigned int *_neighbors, unsigned int _neighborIndex, unsigned int _numNeighbors);
  inline signed int compete_in_tournament(unsigned int _hash, unsigned int _inMIS, unsigned char *_tournamentArray, unsigned int _successorID);
  inline void mark_vertex(Vertex *_vertices, unsigned char *_tournamentArray, int * mis_array, unsigned int *_neighbors, signed int _inMIS);
  inline signed int update_local_counter(unsigned int _inMIS);

  void print_tournament(unsigned char *_tournamentArray, unsigned int _vertexID);
};



#include <pthread.h>
#include <cilk/reducer_opor.h>


#endif /* GLOBALS_H_ */
