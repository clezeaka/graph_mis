#ifndef ORDERING_H_
#define ORDERING_H_

#include "globals.h"

inline unsigned int mm3hash(unsigned int );

static unsigned int TOP_5_BITMASK = 0xF8000000;
inline unsigned int get_order_random_lg_degree(unsigned int i) {
	unsigned int random = mm3hash(i) & ~TOP_5_BITMASK;
	unsigned int degree = sparse_rep.Starts[i + 1] - sparse_rep.Starts[i];
	unsigned int lgDegree = compute_log(degree);
	//printf("vertex %d has lgDegree %d\n", i, lgDegree);
	return (((lgDegree << (32 - 5)) & TOP_5_BITMASK) | random);
}

inline unsigned int get_order_index(unsigned int i) {
	// output in [0..numRows-1]
	return sparse_rep.numRows - i - 1;
}

inline unsigned int get_order_random(unsigned int i) {
	return rand();
}

inline unsigned int get_order_degree(unsigned int i) {
  return sparse_rep.Starts[i+1] - sparse_rep.Starts[i];
}

inline unsigned int mm3hash(unsigned int i){
	i ^= i >> 16;
	i *= 0x85ebca6b;
	i ^= i >> 13;
	i *= 0xc2b2ae35;
	i ^= i >> 16;
	return i;
}

/**
 * Default method is INPUT ordering
 */
ordering_method_t _parse_ordering_type(std::string order_str) {
	if (order_str.compare("input") == 0) {
		return INPUT;
	}
	if (order_str.compare("random") == 0) {
		return RANDOM;
	}
	if (order_str.compare("random_iterated") == 0) {
		return RANDOM_IT;
	}
	if (order_str.compare("degree") == 0) {
		return DEGREE;
	}
	if (order_str.compare("lgdegree") == 0) {
		return LG_DEGREE;
	}
	return INPUT;
}

#endif /* ORDERING_H_ */
