#include "tournament.h"

inline signed long Vertex::update_local_counter(unsigned long _inMIS)
{
    while( true ) {
        if(__sync_lock_test_and_set(&mutex,1) == 0 ) {
            if(_inMIS == 1) {
                mutex = 0;
                return 0;
            }
            counter--;
            if( counter == 0 ) {
                mutex = 0;
                return 1;
            } else {
                mutex = 0;
                return -1;
            }
        } else {
            //while( mutex != 0 );
        }
    }
}

inline signed long LeafClass::update_leaf_counter(unsigned long _inMIS)
{
    while( true ) {
        if(__sync_lock_test_and_set(&mutex, 1) == 0 ) {
            if(_inMIS == 1) {
              counter = 0;
              mutex = 0;
              return 0;
            }
            if( counter == 1 ) {
                mutex = 0;
                return 1;
            } else {
                counter--;
                mutex = 0;
                return -1;
            }
        } else {
            //while( mutex != 0 );
        }
    }
}

inline signed long Vertex::compete_in_tournament(
    unsigned int _hash, // of incoming gladiator
    unsigned int _inMIS,
    unsigned char *_tournamentArray,
    unsigned int _successorID)
{
    unsigned int numNeighbors = sparse_rep.Starts[_successorID + 1] - sparse_rep.Starts[_successorID];

    if( numNeighbors <= COUNTER_THRESHOLD ) { // just use the local counter
        return update_local_counter(_inMIS);
    } else { //full tournament
        unsigned int size = 1 << (unsigned int) logSize;
        unsigned char *tournament = &_tournamentArray[(edgeIndex+7)&(~0x07)];
        unsigned long *bitColors = (unsigned long *) tournament;
        unsigned int index = (_hash & (size - 1));
        LeafClass *leaf = (LeafClass *) &(bitColors[size]);
        _inMIS = leaf[index].update_leaf_counter(_inMIS);
        if( _inMIS != -1 ) { // compete in tournament
            unsigned int type = 0;
            if (_inMIS) {
                type = 1; // lazy
            } else {
                type = 2; // eager
            }
            index += size;
            while( index > 1 ) {
                index = (index >> 1);
                if( bitColors[index] != 0 ) {

                } else {
                    unsigned long getAndSetResult = __sync_lock_test_and_set(&bitColors[index], _inMIS);
                    if( getAndSetResult != 0 || type == 2) { // Move up in the tournament
                    
                    } else {
                        return -1;
                    }
                }
            }
            return _inMIS;
        }
        else
            return 0L;
    }
}

inline void Vertex::mark_vertex(
    Vertex *_vertices,
    unsigned char *_tournamentArray,
    int * mis_array,
    unsigned int *_neighbors,
    signed long _inMIS)
{
    unsigned char *tournament = &_tournamentArray[(edgeIndex+7)&(~0x07)];
    if( numPredecessors == 0 ) {
        finalInMIS = 1;
        mis_array[vertexID] = 1;
    }
    else {
        finalInMIS = _inMIS;
        mis_array[vertexID] = _inMIS;
    }
    unsigned int *successors = &(_neighbors[edgeIndex]);

    /*
    if( SOFTWARE_PREFETCHING_FLAG1 ) {
        for( unsigned int i = 0; i < MIN(numSuccessors,PREFETCH_DISTANCE); i++ ) {
            __builtin_prefetch(&_vertices[successors[i]], 1, 0);
        }
    }
    */

    if( numSuccessors < CILK_FOR_THRESHOLD ) {
        for( int i = 0; i < numSuccessors; i++ ) {
            /*
            if( SOFTWARE_PREFETCHING_FLAG1 ) {
                if( i + PREFETCH_DISTANCE < numSuccessors )
                    __builtin_prefetch(&_vertices[successors[i+PREFETCH_DISTANCE]], 1, 0);
            }
            */
            signed long inMIS = _vertices[successors[i]].compete_in_tournament(hashValue, finalInMIS, _tournamentArray, successors[i]);
            if( inMIS != -1 ) {
                cilk_spawn _vertices[successors[i]].mark_vertex(_vertices, _tournamentArray, mis_array, _neighbors, inMIS);
            }
        }
    }
    else {
        cilk_for( int i = 0; i < numSuccessors; i++ ) {
            /*
            if( SOFTWARE_PREFETCHING_FLAG1 ) {
                if( i + PREFETCH_DISTANCE < numSuccessors )
                    __builtin_prefetch(&_vertices[successors[i+PREFETCH_DISTANCE]], 1, 0);
            }
            */
            signed long inMIS = _vertices[successors[i]].compete_in_tournament(hashValue, finalInMIS, _tournamentArray, successors[i]);
            if( inMIS != -1 ) {
                _vertices[successors[i]].mark_vertex(_vertices, _tournamentArray, mis_array, _neighbors, inMIS);
            }
        }
    }
}

void Vertex::vertex_init(
    Vertex *_vertices,
    unsigned char *_tournamentArray,
    unsigned int _vertexID,
    unsigned int *_neighbors,
    unsigned int _neighborIndex,
    unsigned int _numNeighbors)
{
    vertexID = _vertexID;
    hashValue = hash_vertex_id(_vertexID);
    unsigned int myOrder;// = ORDER(_vertexID);
    numPredecessors = 0;
    numSuccessors = 0;
    mutex = 0;
    counter = 0;
    finalInMIS = 0L;
    edgeIndex = _neighborIndex;
    if(_numNeighbors == 0) return;

    myOrder = _vertexID;

    // Partition predecessors and successors
    unsigned int *successor = &_neighbors[_neighborIndex];
    unsigned int *predecessor = &_neighbors[_neighborIndex + _numNeighbors - 1];

    for(unsigned int i = 0; i < _numNeighbors; i++) {
        unsigned int nbr = *successor;
        unsigned int hisOrder; // = ORDER(nbr);
        unsigned int hisIndex = nbr;
        hisOrder = hisIndex;

        if( myOrder < hisOrder || ((hisOrder == myOrder) && (hisIndex < _vertexID))) {
            *successor = *predecessor;
            *predecessor = nbr;
            predecessor--;
            numPredecessors++;
        } else {
            successor++;
            numSuccessors++;

        }
    }

    if( _numNeighbors <= COUNTER_THRESHOLD ) {
        counter += numPredecessors;
        return;
    }
    else {
        logSize = log_floor(numPredecessors/NUM_LEAF_MEMBERS);
        unsigned int size = (1 << logSize);
        unsigned int mask = size - 1;
        // we get 1 byte per neighbor in the tournament array...
        unsigned char *tournament = &(_tournamentArray[(_neighborIndex+7)&(~0x07)]);
        unsigned long *bitColors = (unsigned long *) tournament;


        if( logSize >= 3 ) { // So, tournament is multiple of 8 bytes...
            for( unsigned int i = 0; i < (size*(sizeof(unsigned long) + sizeof(LeafClass)))>>3; i++ )
                bitColors[i] = 0;
        }
        else {
            for( unsigned int i = 0; i < size*(sizeof(unsigned long) + sizeof(LeafClass)); i++ )
                tournament[i] = 0;
        }

        predecessor = &_neighbors[_neighborIndex + numSuccessors];
        LeafClass *leaf = (LeafClass *) &(bitColors[size]);

        for (unsigned int i = 0; i < numPredecessors; i++) {
            leaf[hash_vertex_id(predecessor[i]) & mask].counter++;
        }

        for( unsigned int i = 0; i < size; i++ ) {
            if( leaf[i].counter == 0 ) { // we need to pre-populate the tournament in this case
                unsigned int j = (size + i) >> 1;
                while( bitColors[j] > 0 )
                    j = j >> 1;
                bitColors[j] = OVERFLOW_BIT_COLOR;
            }
        }
    }
}


void Vertex::print_tournament(unsigned char *_tournamentArray, unsigned int _vertexID) {
    printf("vid: %8u hash: %8u pred: %7u succ: %7u\n", vertexID, hashValue,numPredecessors, numSuccessors);
    printf("index: %8u cntr: %3u lgsz: %2u mtx: %2u bitColor: %#lx\n", edgeIndex, counter, logSize, mutex, finalInMIS);

    unsigned int numNeighbors = sparse_rep.Starts[_vertexID + 1] - sparse_rep.Starts[_vertexID];

    if( numNeighbors > COUNTER_THRESHOLD ) {
        unsigned int size = 1 << (unsigned int) logSize;
        unsigned long *bitColors = (unsigned long *) &_tournamentArray[(edgeIndex+7)&(~0x07)];
        LeafClass *leaf = (LeafClass *) (bitColors + size);
        unsigned int tmpCnt = 0;
        printf("BitColor:\n");
        for( unsigned int i = 0; i <= size/16; i++ ) {
            for( unsigned int j = 0; j < MIN(16,size); j++ ) {
                printf("%#lx ", bitColors[tmpCnt++]);
            }
            printf("\n");
        }
        tmpCnt = 0;
        printf("Leaves:\n");
        for( unsigned int i = 0; i <= size/8; i++ ) {
            for( unsigned int j = 0; j < MIN(8,size); j++ ) {
                leaf[tmpCnt++].print_leaf();
            }
            printf("\n");
        }
    }
}

void LeafClass::print_leaf() {
    printf("%u %u %#lx   ", mutex, counter, bitColor);
}

