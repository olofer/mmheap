/*
 * mmheap.h
 *
 * Rudimentary C++ template implementation of a min-max-heap.
 *
 * This MinMaxHeap is not dynamically allocating;
 * it is a fixed-max-size container.
 *
 * Each object in the heap has a pair of properties: ("value", "index").
 * The heap property is based on "value", and each object sorted by value
 * has an associated property "index".
 *
 * Implementation based on original reference:
 *    Atkinson, Sack, Santoro, Strothotte,
 *    "Min-Max Heaps and Generalized Priority Queues",
 *      Communications of the ACM October 1986, Vol 29, No 10.
 *
 */

#ifndef __MMHEAP_H__
#define __MMHEAP_H__

/* Below aux. template programs operate on simple linear arrays */

namespace MinMaxHeapAux
{

// for i:   0,1,2,3,4,5,6,7,8,9,...
// returns: 0,1,2,2,3,3,3,3,4,4,...
// useful for checking if a level is of min- or max-type
static inline int __msbpos(int i) {
  if (i <= 0) return 0;
  int r = 1;
  while (i >>= 1) r++;
  return r;
}

// 1-based index i
// odd level is min-level (1,3,5,...)
// even level is max-level (2,4,6,...)
static inline int __isminlevel(int i) {
  if (__msbpos(i) & 1) return 1;
  return 0;
}

template<class V, class I>
static inline void __ab_swap(V *A, I *B, int i, int j) {
  V tmpa = A[j];
  A[j] = A[i];
  A[i] = tmpa;
  I tmpb = B[j];
  B[j] = B[i];
  B[i] = tmpb;
}

// bubble up is used for insertion

template<class V, class I>
static void __bubble_up_min(V *A, I *B, int i) {
  int grandparenti = (i >> 2);
  if (grandparenti) {
    if (A[i - 1] < A[grandparenti - 1]) {
      __ab_swap<V, I>(A, B, i - 1, grandparenti - 1);
      __bubble_up_min<V, I>(A, B, grandparenti);
    }
  }
}

template<class V, class I>
static void __bubble_up_max(V *A, I *B, int i) {
  int grandparenti = (i >> 2);
  if (grandparenti) {
    if (A[i - 1] > A[grandparenti - 1]) {
      __ab_swap<V, I>(A, B, i - 1, grandparenti - 1);
      __bubble_up_max<V, I>(A, B, grandparenti);
    }
  }
}

template<class V, class I>
static void __bubble_up(V *A, I *B, int i) {
  int parenti = (i >> 1);
  if (__isminlevel(i)) {
    if (parenti) {
      if (A[i - 1] > A[parenti - 1]) {
        __ab_swap<V, I>(A, B, i - 1, parenti - 1);
        __bubble_up_max<V, I>(A, B, parenti);
      } else {
        __bubble_up_min<V, I>(A, B, i);
      }
    } else {
      __bubble_up_min<V, I>(A, B, i);
    }
  } else {
    if (parenti) { 
      if (A[i - 1] < A[parenti - 1]) {
        __ab_swap<V, I>(A, B, i - 1, parenti - 1);
        __bubble_up_min<V, I>(A, B, parenti);
      } else {
        __bubble_up_max<V, I>(A, B, i);
      }
    } else {
      __bubble_up_max<V, I>(A, B, i);
    }
  }
}

// trickle down is used for removal

template <class V, class I>
static void __trickle_down_min(V *A, I *B, int i, int maxi) {
  int m;
  int lchild = i << 1;    // children
  if (lchild>maxi)
    return;  // no children at all; nothing to do
  int rchild = lchild + 1;
  if (rchild <= maxi) {
    // i has two children
    if (A[lchild - 1] < A[rchild - 1]) {
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
    if (llchild <= maxi) {
      if (A[llchild - 1] < A[m - 1]) {
        m = llchild;
      }
    }
    if (lrchild <= maxi) {
      if (A[lrchild - 1] < A[m - 1]) {
        m = lrchild;
      }
    }
    if (rlchild <= maxi) {
      if (A[rlchild - 1] < A[m - 1]) {
        m = rlchild;
      }
    }
    if (rrchild <= maxi) {
      if (A[rrchild - 1] < A[m - 1]) {
        m = rrchild;
      }
    }
  } else {
    // i has only one child
    m = lchild;
  }
  // at this point m is the index of the minimum-value child or grandchild
  if (m > rchild) {
    // m is a grandchild
    if (A[m - 1] < A[i - 1]) {
      __ab_swap<V, I>(A, B, i - 1, m - 1);
      int parentm = m >> 1;
      if (A[m - 1] > A[parentm - 1]) {
        __ab_swap<V, I>(A, B, m - 1, parentm - 1);
      }
      __trickle_down_min<V, I>(A, B, m, maxi);
    }
  } else {
    // m is child
    if (A[m - 1] < A[i - 1]) {
      __ab_swap<V, I>(A, B, i - 1, m - 1);
    }
  }
}

template <class V, class I>
static void __trickle_down_max(V *A, I *B, int i, int maxi) {
  int m;
  int lchild = i << 1;    // children
  if (lchild > maxi)
    return;  // no children at all; nothing to do
  int rchild = lchild + 1;
  if (rchild <= maxi) {
    // i has two children
    if (A[lchild - 1] > A[rchild - 1]) {
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
    if (llchild <= maxi) {
      if (A[llchild - 1] > A[m - 1]) {
        m = llchild;
      }
    }
    if (lrchild <= maxi) {
      if (A[lrchild - 1] > A[m - 1]) {
        m = lrchild;
      }
    }
    if (rlchild <= maxi) {
      if (A[rlchild - 1] > A[m - 1]) {
        m = rlchild;
      }
    }
    if (rrchild <= maxi) {
      if (A[rrchild - 1] > A[m - 1]) {
        m = rrchild;
      }
    }
  } else {
    // i has only one child
    m = lchild;
  }
  
  // at this point m is the index of the maximum-value child or grandchild
  if (m > rchild) {
    // m is a grandchild
    if (A[m - 1] > A[i - 1]) {
      __ab_swap<V, I>(A, B, i - 1, m - 1);
      int parentm = m >> 1;
      if (A[m - 1] < A[parentm - 1]) {
        __ab_swap<V, I>(A, B, m - 1, parentm - 1);
      }
      __trickle_down_max<V, I>(A, B, m, maxi);
    }
  } else {
    // m is child
    if (A[m - 1] > A[i - 1]) {
      __ab_swap<V, I>(A, B, i - 1, m - 1);
    }
  }
}

template <class V, class I>
static void __trickle_down(V *A, I *B, int i, int maxi) {
  if (__isminlevel(i)) {
    __trickle_down_min<V, I>(A, B, i, maxi);
  } else {
    __trickle_down_max<V, I>(A, B, i, maxi);
  }
}

} // end aux. namespace

template <class V, class I>
class MinMaxHeap
{
public:
  MinMaxHeap(int m) {
    if (m <= 0) m = 1; // Construct something valid always
    value = new V [m];
    index = new I [m];
    maxlength = m;
    length = 0;
  }

  MinMaxHeap(const MinMaxHeap& c) {
    int m = c.MaxLength();
    value = new V [m];
    index = new I [m];
    maxlength = m;
    int l = c.Length();
    for (int i = 0; i < l; i++) {
      value[i] = c.value[i];
      index[i] = c.index[i];
    }
    length = l;
  }

  ~MinMaxHeap() {
    delete[] value;
    delete[] index;
  }

  int Length() const { return length; }
  int MaxLength() const { return maxlength; }

  /* O(1) peek operations */

  bool PeekMinValue(V *v) const {
    if (length == 0 || v == nullptr) return false;
    *v = value[0];
    return true;
  }

  bool PeekMaxValue(V *v) const {
    if (length == 0 || v == nullptr) return false;
    if (length == 1) {
      *v = value[0];
    } else if (length == 2) {
      *v = value[1];
    } else {
      if (value[1] >= value[2]) {
        *v = value[1];
      } else {
        *v = value[2];
      }
    }
    return true;
  }

  bool PeekMinIndex(I *i) const {
    if (length == 0 || i == nullptr) return false;
    *i = index[0];
    return true;
  }

  bool PeekMaxIndex(I *i) const {
    if (length == 0 || i == nullptr) return false;
    if (length == 1) {
      *i = index[0];
    } else if (length == 2) {
      *i = index[1];
    } else {
      if (value[1] >= value[2]) {
        *i = index[1];
      } else {
        *i = index[2];
      }
    }
    return true;
  }

  bool PeekMin(V *v, I *i) const {
    return PeekMinValue(v) || PeekMinIndex(i);
  }

  bool PeekMax(V *v, I *i) const {
    return PeekMaxValue(v) || PeekMaxIndex(i);
  }

  /* Insert and remove ops are O(log(k)), k = length */

  bool Insert(V v, I i) {
    if (length == maxlength) return false;
    V *A = value;
    I *B = index;
    int j = length;
    length++;
    A[j] = v;
    B[j] = i;
    MinMaxHeapAux::__bubble_up<V, I>(A, B, length);
    return true;
  }

  bool RemoveMin() {
    // replace min value (root) with last heap element then trickle down
    if (length == 0) return false;
    V *A = value;
    I *B = index;
    V last_a = A[length - 1];
    I last_b = B[length - 1];
    length--;  // remove last element
    A[0] = last_a;
    B[0] = last_b;  // reinsert at root
    MinMaxHeapAux::__trickle_down<V, I>(A, B, 1, length);  // restore heap property
    return true;
  }

  bool RemoveMax() {
    // replace max value (always a child of root or the root) with last heap element then trickle down
    if (length == 0) return false;
    V *A = value;
    I *B = index;
    V last_a = A[length - 1];
    I last_b = B[length - 1];
    int iins;
    if (length == 1) {
      iins = 1;
    } else if (length == 2) {
      iins = 2;
    } else {
      if (value[1] >= value[2]) {
        iins = 2;
      } else {
        iins = 3;
      }
    }
    length--;    // remove last element
    A[iins - 1] = last_a;    // reinsert at position where the max was previously
    B[iins - 1] = last_b;
    MinMaxHeapAux::__trickle_down<V, I>(A, B, iins, length);  // restore heap property
    return true;
  }

private:
  V* value;
  I* index;
  int length;
  int maxlength;
};

#endif
