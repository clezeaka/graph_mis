#ifndef PARALLEL_H_
#define PARALLEL_H_

// cilkarts cilk++
#if defined(CILK)
#include <cilk.h>
#define parallel_main cilk_main
#define parallel_for cilk_for
#define parallel_for_1 _Pragma("cilk_grainsize = 1") cilk_for
#define parallel_for_256 _Pragma("cilk_grainsize = 256") cilk_for

// intel cilk+
#elif defined(CILKP)
#include <cilk/cilk.h>
#define parallel_for cilk_for
#define parallel_main main
#define parallel_for_1 _Pragma("cilk grainsize = 1") cilk_for
#define parallel_for_256 _Pragma("cilk grainsize = 256") cilk_for

// openmp
#elif defined(OPENMP)
#include <omp.h>
#define cilk_spawn
#define cilk_sync
#define parallel_main main
#define parallel_for _Pragma("omp parallel for") for
#define parallel_for_1 _Pragma("omp parallel for schedule (static,1)") for
#define parallel_for_256 _Pragma("omp parallel for schedule (static,256)") for

// c++
#else
#undef cilk_spawn
#define cilk_spawn
#undef cilk_sync
#define cilk_sync
#undef cilk_for
#define cilk_for for
#define parallel_main main
#define parallel_for for
#define parallel_for_1 for
#define parallel_for_256 for
#undef __cilkrts_set_param
#define __cilkrts_set_param(dummy, definition)

#endif

#include <limits.h>

#if defined(LONG)
typedef long intT;
typedef unsigned long uintT;
#define INT_T_MAX LONG_MAX
#else
typedef int intT;
typedef unsigned int uintT;
#define INT_T_MAX INT_MAX
#endif

#endif // PARALLEL_H_
