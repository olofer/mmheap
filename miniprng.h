#ifndef __MINIPRNG_H__
#define __MINIPRNG_H__

/*
 * Source: W. H. Press et al., Numerical Recipes in C, 2nd edition, 1992, Cambridge University Press
 */

#define __RAN0_IA 16807
#define __RAN0_IM 2147483647
#define __RAN0_AM (1.0/__RAN0_IM)
#define __RAN0_IQ 127773
#define __RAN0_IR 2836
#define __RAN0_MASK 123459876

/*
“Minimal” random number generator of Park and Miller. Returns a uniform random deviate
between 0.0 and 1.0. Set or reset idum to any integer value (except the unlikely value MASK)
to initialize the sequence; idum must not be altered between calls for successive deviates in
a sequence.
*/

float ran0(long *idum)
{
  long k;
  float ans;
  *idum ^= __RAN0_MASK;
  k = (*idum)/__RAN0_IQ;
  *idum = __RAN0_IA * (*idum - k * __RAN0_IQ) - __RAN0_IR*k;
  if (*idum < 0) *idum += __RAN0_IM;
  ans = __RAN0_AM * (*idum);
  *idum ^= __RAN0_MASK;
  return ans;
}

#define __RAN1_IA 16807
#define __RAN1_IM 2147483647
#define __RAN1_AM (1.0/__RAN1_IM)
#define __RAN1_IQ 127773
#define __RAN1_IR 2836
#define __RAN1_NTAB 32
#define __RAN1_NDIV (1+(__RAN1_IM-1)/__RAN1_NTAB)
#define __RAN1_EPS 1.2e-7
#define __RAN1_RNMX (1.0-__RAN1_EPS)

/*
 “Minimal” random number generator of Park and Miller with Bays-Durham shuffle and added
safeguards. Returns a uniform random deviate between 0.0 and 1.0 (exclusive of the endpoint
values). Call with idum a negative integer to initialize; thereafter, do not alter idum between
successive deviates in a sequence. RNMX should approximate the largest floating value that is
less than 1.
 */

float ran1(long *idum)
{
  int j;
  long k;
  static long iy = 0;
  static long iv[__RAN1_NTAB];
  float temp;
  if (*idum <= 0 || !iy) {
    if (-(*idum) < 1) *idum = 1; 
      else *idum = -(*idum);
    for (j = __RAN1_NTAB + 7; j >= 0; j--) {
      k = (*idum) / __RAN1_IQ;
      *idum = __RAN1_IA * (*idum - k * __RAN1_IQ) - __RAN1_IR * k;
      if (*idum < 0) *idum += __RAN1_IM;
      if (j < __RAN1_NTAB) iv[j] = *idum;
    }
    iy=iv[0];
  }
  k = (*idum) / __RAN1_IQ;
  *idum = __RAN1_IA * (*idum - k * __RAN1_IQ) - __RAN1_IR * k;
  if (*idum < 0) *idum += __RAN1_IM;
  j = iy / __RAN1_NDIV;
  iy = iv[j];
  iv[j] = *idum;
  if ((temp = __RAN1_AM * iy) > __RAN1_RNMX) return __RAN1_RNMX;
    else return temp;
}

#endif
