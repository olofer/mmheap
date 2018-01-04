/*
 * fastclock.h
 *
 * Basic interface to clock_gettime() <time.h>
 * to get nanosecond resolution timing (supposedly).
 *
 */
 
#ifndef __FASTCLOCK_H__
#define __FASTCLOCK_H__

typedef struct timespec fclk_timespec;

static inline void fclk_get_resolution(fclk_timespec *prs) {
  /* You probably want to see *prs field values: tv_sec=0, tv_nsec=1 */
  clock_getres(CLOCK_MONOTONIC, prs);
}

static inline void fclk_timestamp(fclk_timespec *ptm) {
  clock_gettime(CLOCK_MONOTONIC, ptm);
}

static inline double fclk_time(fclk_timespec *ptm) {
  return ((double)(ptm->tv_sec)+1.0e-9*(double)(ptm->tv_nsec));
}

static inline double fclk_delta_timestamps(fclk_timespec *ptm1, fclk_timespec *ptm2) {
  /* stamp2 - stamp1 return counter diff as double [sec] */
  double delta_sec=(double)(ptm2->tv_sec)-(double)(ptm1->tv_sec);
  double delta_nsec=(double)(ptm2->tv_nsec)-(double)(ptm1->tv_nsec);
  return (delta_sec+1.0e-9*delta_nsec);
}

static inline double fclk_delta_timestamps__(
  fclk_timespec *ptm1, fclk_timespec *ptm2, fclk_timespec *prs) {
  /* ignore prs and return delta as above; compatibility */
  return fclk_delta_timestamps(ptm1, ptm2);
}

#endif

