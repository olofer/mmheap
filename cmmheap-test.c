/* 
 * Simple applications of the min-max-heap.
 * (demonstration, testing, benchmarking, and debugging)
 */

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "fastclock.h"
#include "miniprng.h"

#include "cmmheap.h"

long rnd_state = -1.0;

// Sorting functions; ksmallest and klargest, derived from the min-max-heap; O(n log k). Returns min(n,k)
int ksmallest(double *x,int n,int k,double *xk,int *ik) {
  // x is a length-n array of real numbers, find the k smallest numbers
  // and store them sorted into xk and their indices in ik.
  minmaxheap *pheap = mmheap_create(k);
  // iterate through the array and maintain the size-k heap
  int i;
  for (i=0;i<n;i++) {
    if (mmheap_getlength(pheap)==k) {
      // need to remove the largest element before inserting the next, if it should be inserted at all
      if (x[i]<mmheap_peekmax_value(pheap)) {
        mmheap_removemax(pheap);
        mmheap_insert(pheap,x[i],i);
      }
    } else {
      mmheap_insert(pheap,x[i],i);
    }
  }
  // empty heap contents to output arrays
  i = 0;
  while (mmheap_getlength(pheap)) {
    xk[i] = mmheap_peekmin_value(pheap);
    ik[i] = mmheap_peekmin_index(pheap);
    mmheap_removemin(pheap);
    i++;
  }
  mmheap_destroy(pheap);
  return i;
}

int klargest(double *x,int n,int k,double *xk,int *ik) {
  // x is a length-n array of real numbers, find the k largest numbers
  // and store them sorted into xk and their indices in ik.
  minmaxheap *pheap = mmheap_create(k);
  // iterate through the array and maintain the size-k heap
  int i;
  for (i=0;i<n;i++) {
    if (mmheap_getlength(pheap)==k) {
      // need to remove the largest element before inserting the next, if it should be inserted at all
      if (x[i]>mmheap_peekmin_value(pheap)) {
        mmheap_removemin(pheap);
        mmheap_insert(pheap,x[i],i);
      }
    } else {
      mmheap_insert(pheap,x[i],i);
    }
  }
  // empty heap contents to output arrays
  i = 0;
  while (mmheap_getlength(pheap)) {
    xk[i] = mmheap_peekmax_value(pheap);
    ik[i] = mmheap_peekmax_index(pheap);
    mmheap_removemax(pheap);
    i++;
  }
  mmheap_destroy(pheap);
  return i;
}

// Benchmarking

int __qsort_comparefun(const void* a,const void* b)
{
  double va = *(double*) a;
  double vb = *(double*) b;
  return (va > vb) - (va < vb);
}

void test_smallest_and_largest(int n,int k)
{
  // simple test of the k-smallest/largest O(n log k) sorting routines

  double *x = (double *)malloc(sizeof(double)*n);  
  double *y = (double *)malloc(sizeof(double)*n);
  int i;
  for (i=0;i<n;i++) {
  //  x[i] = ((double)rand())/RAND_MAX;
    x[i] = (double) ran0(&rnd_state);
    y[i] = x[i];
  }
  qsort (y,n,sizeof(double),__qsort_comparefun);  // verify result against quicksort
  
  double *xk=(double *)malloc(sizeof(double)*k);
  int *ik=(int *)malloc(sizeof(int)*k);
  
  ksmallest(x,n,k,xk,ik);
  
  printf("*** smallest ***\n");
  for (i=0;i<k;i++) {
    printf("qsort #%i: %f\t ksmallest #%i, %f, at %i\n",i,y[i],i,xk[i],ik[i]);
  }
  
  klargest(x,n,k,xk,ik);
  
  printf("*** largest ***\n");
  for (i=0;i<k;i++) {
    printf("qsort #%i: %f\t klargest #%i, %f, at %i\n",n-1-i,y[n-1-i],i,xk[i],ik[i]);
  }
  
  free(x);
  free(y);
  free(xk);
  free(ik);
}

void compare_mmheap_to_qsort(int n,int k)
{
  // Time the finding of the k smallest numbers from n random numbers via
  // (1) qsort of full array and k first elements; nominally O[n log n]
  // (2) ksmallest-type algorithm; nominally O[n log k]
  // print the timing results to the console; also verify the equivalence of the results.

  fclk_timespec __tic, __toc;
  double elap_qsort = 0.0f;
  double elap_ksort = 0.0f;
  
  minmaxheap *pheap = mmheap_create(k);
  
  double *x = (double *)malloc(sizeof(double)*n);  
  double *y = (double *)malloc(sizeof(double)*n);
  int i;
  
  for (i=0;i<n;i++) {
//    x[i] = ((double)rand())/RAND_MAX;
    x[i] = (double) ran0(&rnd_state);
    y[i] = x[i];
  }
  
  // run a qsort in-place
  fclk_timestamp(&__tic);
  qsort (y,n,sizeof(double),__qsort_comparefun);
  fclk_timestamp(&__toc);
  elap_qsort = fclk_delta_timestamps(&__tic, &__toc);
  printf("[qsort] elapsed: %f us\n", elap_qsort * 1.0e6);

  //for (i=0;i<k;i++) {
  //  printf("qsort #%i: %f\n",i+1,y[i]);
  //}
  
  fclk_timestamp(&__tic);
  // iterate through the array and maintain the size-k heap
  for (i=0;i<n;i++) {
    if (mmheap_getlength(pheap)==k) {
      // need to remove the largest element before inserting the next, if it should be inserted at all
      double maxheapval = mmheap_peekmax_value(pheap);
      if (x[i]<maxheapval) {
        mmheap_removemax(pheap);
        if (!mmheap_insert(pheap,x[i],i)) {
          printf("insert to heap failed for (%f,%i).\n",x[i],i);
        }
      }
    } else {
      if (!mmheap_insert(pheap,x[i],i)) {
        printf("insert to heap failed for (%f,%i).\n",x[i],i);
      }
    }
  }
  fclk_timestamp(&__toc);
  elap_ksort = fclk_delta_timestamps(&__tic, &__toc);
  printf("[ksort] elapsed: %f us (excluded malloc/free)\n", elap_ksort * 1.0e6);
  
  // print out smallest min(n,k) elements
  i = 0;
  while (mmheap_getlength(pheap)) {
  
    if (y[i]!=x[mmheap_peekmin_index(pheap)] || y[i]!=mmheap_peekmin_value(pheap)) {
      printf("sorting mismatch found @ pos = %i\n",i+1);
    }
    
    i++;
  
  //  printf("ksort #%i: min=(%f,%i)\n",i,mmheap_peekmin_value(pheap),mmheap_peekmin_index(pheap));
    mmheap_removemin(pheap);
  }
  
  mmheap_destroy(pheap);
  free(x);
  free(y);
}

/* MAIN */

int main(int argc, char **argv)
{
  if (argc != 3) {
    printf("usage: %s n k\n", argv[0]);
    return 1;
  }

  int n = atoi(argv[1]);
  int k = atoi(argv[2]);

  int kmax = 100;

  if (n <= 0 || k <= 0 || k > n) {
    printf("n, k arguments not allowed\n");
    return 1;
  }

  // initialize PRNG
  fclk_timespec __tic;
  fclk_timestamp(&__tic);
  rnd_state = (long) round(1.0e3 * fclk_time(&__tic));
  printf("rnd_state = %li\n", rnd_state);

  if (k <= kmax) {
    test_smallest_and_largest(n, k);
  } else {
    printf("skipped smallest/largest printout check since k > %i\n", kmax);
  }

  compare_mmheap_to_qsort(n, k);

  return 0;
}
