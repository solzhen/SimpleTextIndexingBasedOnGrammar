/* 
 *  Copyright (c) 2011 Shirou Maruyama
 * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 * 
 *   1. Redistributions of source code must retain the above Copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above Copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 */

#ifndef REPAIR_H
#define REPAIR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "basics.h"

typedef struct Sequence {
  CODE code;
  // next position such that the same substring begins at W[next]
  // and all three occurrences are captured by the same reference
  // (i.e. there is a variable in the grammar generating the string).
  uint next;
  // previous position such that the same substring begins at W[prev]
  // and all three occurrences are captured by the same reference
  // (i.e. there is a variable in the grammar generating the string).
  uint prev;
} SEQ;

typedef struct Pair {
  CODE left;
  CODE right;
  uint freq; // frequency of the pair
  uint f_pos; // first position in W where the string represented by PAIR occurs
  uint b_pos; // last position in W where the string represented by PAIR occurs
  struct Pair *h_next; // next pair with the same hash
  struct Pair *p_next; // next pair with the same frequency
  struct Pair *p_prev; // previous pair with the same frequency
} PAIR;

typedef struct RePair_data_structures {
  uint txt_len;
  SEQ *seq;
  uint num_pairs;
  uint h_num;
  PAIR **h_first; // hash table, h_first[i] is the first pair with hash value i
  uint p_max;
  PAIR **p_que; // priority queue, p_que[i] is the first pair with frequency i
} RDS;

typedef struct Rule {
  CODE left;
  CODE right;
} RULE;

typedef struct Dictionary {
  uint txt_len;
  uint num_rules;
  RULE *rule;
  uint seq_len;
  CODE *comp_seq;
  uint buff_size;
} DICT;

DICT *RunRepair(FILE *input);
void DestructDict(DICT *dict);
void OutputGeneratedCFG(DICT *dict, FILE *output);

#endif
