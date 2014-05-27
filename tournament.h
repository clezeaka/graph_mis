#include <assert.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <ctime>
#include <cmath>
#include <string>

#include <cilk/cilk.h>
#include "graphUtils.h"
#include "graphIO.h"
#include "ordering.h"
#define ALL_ONES 0xFFFFFFFFFFFFFFFF
#define CILK_FOR_THRESHOLD 128
#define PREFETCH_DISTANCE 4
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define SOFTWARE_PREFETCHING_FLAG1 true
#define WCH_DEBUG false
#define PRINT_TOURNAMENT 1673
#define COUNTER_THRESHOLD 256
#define NUM_LEAF_MEMBERS 8

inline unsigned int hash_vertex_id(unsigned int _id) {
  const unsigned int randVal = 0xF1807D63;
  return ((randVal * _id) >> 16)*randVal;
}

inline bool is_successor(unsigned int _myHash, unsigned int _hisHash, unsigned int _myID, unsigned int _hisID) {
  if( _myHash != _hisHash )
    return _myHash > _hisHash;
  else
    return _myID < _hisID;
}


inline int log_floor(unsigned int _num) {
  return 31 - __builtin_clz(_num);
}


struct LeafClass
{
  volatile unsigned long inMIS;
  volatile unsigned int counter;// TODO use int for counter and aux_counter (init counter), use top bit of counter for mutex
  volatile unsigned short mutex;

  inline signed int update_leaf_counter(unsigned int _inMIS);

  void print_leaf();
};

