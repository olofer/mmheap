/*
 * cmmheap.h
 * rudimentary plain-c implementation of a min-max-heap.
 *
 * each object in the heap has a pair of properties: ("value","index").
 * the heap property is based on "value".
 *
 * implementation based on original source:
 *    Atkinson, Sack, Santoro, Strothotte,
 *    "Min-Max Heaps and Generalized Priority Queues",
 *      Communications of the ACM October 1986, Vol 29, No 10.
 *
 */

#ifndef __CMMHEAP_H__
#define __CMMHEAP_H__

#ifndef NAN
#define NAN (0.0/0.0)  // not a number
#endif

#define NAI (-1)    // not an index

typedef struct {
  double *value;
  int *index;
  int length;
  int maxlength;
} minmaxheap;

// Create and Destroy procedures

minmaxheap *mmheap_create(int maxlength) {
  minmaxheap *pheap = (minmaxheap *) malloc(sizeof(minmaxheap));
  pheap->value = (double *) malloc(sizeof(double)*maxlength);
  pheap->index = (int *) malloc(sizeof(int)*maxlength);
  pheap->maxlength = maxlength;
  pheap->length = 0;
  return pheap;
}

minmaxheap *mmheap_copy(minmaxheap *psource) {
  int maxlength = psource->maxlength;
  int length = psource->length;
  minmaxheap *pheap = (minmaxheap *) malloc(sizeof(minmaxheap));
  pheap->value = (double *) malloc(sizeof(double)*maxlength);
  pheap->index = (int *) malloc(sizeof(int)*maxlength);
  pheap->maxlength = maxlength;
  pheap->length = length;
  memcpy((void *)(pheap->value),(void *)(psource->value),sizeof(double)*length);
  memcpy((void *)(pheap->index),(void *)(psource->index),sizeof(int)*length);
  return pheap;
}

void mmheap_destroy(minmaxheap *mmheap) {
  free(mmheap->value);
  free(mmheap->index);
  free(mmheap);
}

// Auxiliary functions

// for i:   0,1,2,3,4,5,6,7,8,9,...
// returns: 0,1,2,2,3,3,3,3,4,4,...
// useful for checking if a level is of min- or max-type
static int msbpos(int i) {
  if (i<=0)
    return 0;
  int r = 1;
  while (i >>= 1) {
    r++;
  }
  return r;
}

// 1-based index i
static int isminlevel(int i) {
  if (msbpos(i) & 1) {
    return 1;  // odd level is min-level (1,3,5,...)
  } else {
    return 0;  // even level is max-level (2,4,6,...)
  }
}

static void __ab_swap(double *A,int *B,int i,int j) {
  double tmpa = A[j];
  A[j] = A[i];
  A[i] = tmpa;
  int tmpb = B[j];
  B[j] = B[i];
  B[i] = tmpb;
}

// bubble up is used for insertion

static void __bubble_up_min(double *A,int *B,int i) {
  int grandparenti = (i>>2);
  if (grandparenti) {
    //printf("@min #%i\t#%i\n",i,grandparenti);
    if (A[i-1]<A[grandparenti-1]) {
      __ab_swap(A,B,i-1,grandparenti-1);
      __bubble_up_min(A,B,grandparenti);
    }
  }
}

static void __bubble_up_max(double *A,int *B,int i) {
  int grandparenti = (i>>2);
  if (grandparenti) {
    //printf("@max #%i\t#%i\n",i,grandparenti);
    if (A[i-1]>A[grandparenti-1]) {
      __ab_swap(A,B,i-1,grandparenti-1);
      __bubble_up_max(A,B,grandparenti);
    }
  }
}

static void __bubble_up(double *A,int *B,int i) {
  int parenti = (i>>1);
  if (isminlevel(i)) {
    if (parenti) {
      //printf("@bubble #%i\t#%i\n",i,parenti);
      if (A[i-1]>A[parenti-1]) {
        __ab_swap(A,B,i-1,parenti-1);
        __bubble_up_max(A,B,parenti);
      } else {
        __bubble_up_min(A,B,i);
      }
    } else {
      __bubble_up_min(A,B,i);
    }
  } else {
    if (parenti) { 
      //printf("@bubble #%i\t#%i\n",i,parenti);
      if (A[i-1]<A[parenti-1]) {
        __ab_swap(A,B,i-1,parenti-1);
        __bubble_up_min(A,B,parenti);
      } else {
        __bubble_up_max(A,B,i);
      }
    } else {
      __bubble_up_max(A,B,i);
    }
  }
}

// trickle down is used for removal

static void __trickle_down_min(double *A,int *B,int i,int maxi) {
  int m;
  int lchild = i << 1;    // children
  if (lchild>maxi)
    return;  // no children at all; nothing to do
  int rchild = lchild + 1;
  if (rchild<=maxi) {
    // i has two children
    if (A[lchild-1]<A[rchild-1]) {
      m = lchild;
    } else {
      m = rchild;
    }
    // now find also grandchildren (could exist)
    // no grandchildren exists unless there are two children
    int llchild = lchild << 1;  // grandchildren
    int lrchild = llchild + 1;
    int rlchild = rchild << 1;
    int rrchild = rlchild + 1;
    if (llchild<=maxi) {
      if (A[llchild-1]<A[m-1]) {
        m = llchild;
      }
    }
    if (lrchild<=maxi) {
      if (A[lrchild-1]<A[m-1]) {
        m = lrchild;
      }
    }
    if (rlchild<=maxi) {
      if (A[rlchild-1]<A[m-1]) {
        m = rlchild;
      }
    }
    if (rrchild<=maxi) {
      if (A[rrchild-1]<A[m-1]) {
        m = rrchild;
      }
    }
  } else {
    // i has only one child
    m = lchild;
  }
  
  // at this point m is the index of the minimum-value child or grandchild
  if (m>rchild) {
    // m is a grandchild
    if (A[m-1]<A[i-1]) {
      __ab_swap(A,B,i-1,m-1);
      int parentm = m >> 1;
      if (A[m-1]>A[parentm-1]) {
        __ab_swap(A,B,m-1,parentm-1);
      }
      __trickle_down_min(A,B,m,maxi);
    }
  } else {
    // m is child
    if (A[m-1]<A[i-1]) {
      __ab_swap(A,B,i-1,m-1);
    }
  }
}

static void __trickle_down_max(double *A,int *B,int i,int maxi) {
  int m;
  int lchild = i << 1;    // children
  if (lchild>maxi)
    return;  // no children at all; nothing to do
  int rchild = lchild + 1;
  if (rchild<=maxi) {
    // i has two children
    if (A[lchild-1]>A[rchild-1]) {
      m = lchild;
    } else {
      m = rchild;
    }
    // now find also grandchildren (could exist)
    // no grandchildren exists unless there are two children
    int llchild = lchild << 1;  // grandchildren
    int lrchild = llchild + 1;
    int rlchild = rchild << 1;
    int rrchild = rlchild + 1;
    if (llchild<=maxi) {
      if (A[llchild-1]>A[m-1]) {
        m = llchild;
      }
    }
    if (lrchild<=maxi) {
      if (A[lrchild-1]>A[m-1]) {
        m = lrchild;
      }
    }
    if (rlchild<=maxi) {
      if (A[rlchild-1]>A[m-1]) {
        m = rlchild;
      }
    }
    if (rrchild<=maxi) {
      if (A[rrchild-1]>A[m-1]) {
        m = rrchild;
      }
    }
  } else {
    // i has only one child
    m = lchild;
  }
  
  // at this point m is the index of the maximum-value child or grandchild
  if (m>rchild) {
    // m is a grandchild
    if (A[m-1]>A[i-1]) {
      __ab_swap(A,B,i-1,m-1);
      int parentm = m >> 1;
      if (A[m-1]<A[parentm-1]) {
        __ab_swap(A,B,m-1,parentm-1);
      }
      __trickle_down_max(A,B,m,maxi);
    }
  } else {
    // m is child
    if (A[m-1]>A[i-1]) {
      __ab_swap(A,B,i-1,m-1);
    }
  }
}

static void __trickle_down(double *A,int *B,int i,int maxi) {
  if (isminlevel(i)) {
    __trickle_down_min(A,B,i,maxi);
  } else {
    __trickle_down_max(A,B,i,maxi);
  }
}

// Peek operations (all in O(1)-time)

static int mmheap_getlength(minmaxheap *mmheap) {
  return mmheap->length;
}

static int mmheap_getmaxlength(minmaxheap *mmheap) {
  return mmheap->maxlength;
}

static double mmheap_peekmin_value(minmaxheap *mmheap) {
  if (mmheap->length==0) {
    return NAN;
  }
  return mmheap->value[0];
}

static int mmheap_peekmin_index(minmaxheap *mmheap) {
  if (mmheap->length==0) {
    return NAI;
  }
  return mmheap->index[0];
}

static double mmheap_peekmax_value(minmaxheap *mmheap) {
  if (mmheap->length==0) {
    return NAN;
  }
  if (mmheap->length==1) {
    return mmheap->value[0];
  } else if (mmheap->length==2) {
    return mmheap->value[1];
  } else {
    // at least 3 elements stored; max is always one of the children of the root.
    if (mmheap->value[1]>=mmheap->value[2]) {
      return mmheap->value[1];
    } else {
      return mmheap->value[2];
    }
  }
}

static int mmheap_peekmax_index(minmaxheap *mmheap) {
  if (mmheap->length==0) {
    return NAI;
  }
  if (mmheap->length==1) {
    return mmheap->index[0];
  } else if (mmheap->length==2) {
    return mmheap->index[1];
  } else {
    // at least 3 elements stored; max is always one of the children of the root.
    if (mmheap->value[1]>=mmheap->value[2]) {
      return mmheap->index[1];
    } else {
      return mmheap->index[2];
    }
  }
}

// Insert operation; add the pair (v,i) to the heap; O(log n)

static int mmheap_insert(minmaxheap *mmheap,double v,int i) {
  if (mmheap->length==mmheap->maxlength) {
    return 0;
  }
  double *A = mmheap->value;
  int *B = mmheap->index;
  int j = mmheap->length;
  mmheap->length++;
  
  A[j] = v;
  B[j] = i;
  
  // bubble up uses 1-based indexing
  __bubble_up(A,B,mmheap->length);
  
  return 1;
}

// Remove operations (double-ended); O(log n)

static int mmheap_removemin(minmaxheap *mmheap) {
  // replace min value (root) with last heap element then trickle down
  if (mmheap->length==0)
    return 0;
    
  double *A = mmheap->value;
  int *B = mmheap->index;
  
  double last_a = A[mmheap->length-1];
  int last_b = B[mmheap->length-1];
  mmheap->length--;            // remove last element
  A[0] = last_a;
  B[0] = last_b;              // reinsert at root
  
  __trickle_down(A,B,1,mmheap->length);  // restore heap property
  
  return 1;
}

static int mmheap_removemax(minmaxheap *mmheap) {
  // replace max value (always a child of root or the root) with last heap element then trickle down
  if (mmheap->length==0)
    return 0;
    
  double *A = mmheap->value;
  int *B = mmheap->index;
  
  double last_a = A[mmheap->length-1];
  int last_b = B[mmheap->length-1];
  
  int iins;
  
  if (mmheap->length==1) {
    iins = 1;
  } else if (mmheap->length==2) {
    iins = 2;
  } else {
    // at least 3 elements stored; max is always one of the children of the root.
    if (mmheap->value[1]>=mmheap->value[2]) {
      iins = 2;
    } else {
      iins = 3;
    }
  }
  
  mmheap->length--;    // remove last element
  A[iins-1] = last_a;    // reinsert at position where the max was previously
  B[iins-1] = last_b;
  
  __trickle_down(A,B,iins,mmheap->length);  // restore heap property
  
  return 1;
}

#endif
