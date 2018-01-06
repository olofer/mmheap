/*
 * Test code for C++ template basic min-max-heap/PQ
 * Demonstrates PQ instance with double value and int property (index in a stream).
 * Example shows how the PQ can be used to maintain and extract
 * k-smallest and k-largest values over a vector of n elements.
 * Significantly faster than sorting the full vector.
 *
 * USAGE: ./mmheap-test n k
 *
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <ctime>
#include "fastclock.h"
#include "mmheap.h"

const int kmaxshow = 30;

int main(int argc, char **argv)
{
  if (argc != 3) {
    std::cout << "usage: " << argv[0] << " n k" << std::endl;
    return 1;
  }

  int n = std::atoi(argv[1]);
  int k = std::atoi(argv[2]);

  if (n <= 0 || k <= 0 || k > n) {
    std::cout << "n, k not allowed" << std::endl;
    return 1;
  }

  fclk_timespec __tic, __toc;

  /* First generate n-vector of random doubles using the Mersenne Twister */
  std::mt19937 RandomGenerator;
  auto tp = std::chrono::high_resolution_clock::now();
  auto dn = tp.time_since_epoch();
  unsigned long ul = dn.count();
  RandomGenerator.seed(ul);

  std::uniform_real_distribution<double> U(0.0f, 1.0f);
  std::vector<double> x;

  fclk_timestamp(&__tic);
  for (int i = 0; i < n; i++) {
    double ui = U(RandomGenerator);
    x.push_back(ui);
  }
  fclk_timestamp(&__toc);
  double elap_rand = fclk_delta_timestamps(&__tic, &__toc);
  std::cout << n << " variates took " << elap_rand * 1.0e6 << " us" << std::endl;

  /* Then push elements into 2 different min-max-PQs: k-smallest and k-largest */
  MinMaxHeap<double, int> ksmall(k);
  MinMaxHeap<double, int> klarge(k);

  double tmp = 0.0;

  fclk_timestamp(&__tic);
  for (int i = 0; i < n; i++) {

    // Update ksmall PQ
    if (ksmall.Length() == ksmall.MaxLength()) {
      ksmall.PeekMaxValue(&tmp);
      if (x[i] < tmp) {
        ksmall.RemoveMax();
        ksmall.Insert(x[i], i);
      }
    } else {
      ksmall.Insert(x[i], i);
    }

    // Update klarge PQ
    if (klarge.Length() == klarge.MaxLength()) {
      klarge.PeekMinValue(&tmp);
      if (x[i] > tmp) {
        klarge.RemoveMin();
        klarge.Insert(x[i], i);
      }
    } else {
      klarge.Insert(x[i], i);
    }
    
  }
  fclk_timestamp(&__toc);
  double elap_ksort = fclk_delta_timestamps(&__tic, &__toc);
  std::cout << "2x ksort() took " << elap_ksort * 1.0e6 << " us" << std::endl;

  /* Then create a sorted version of this vector using std::sort */
  fclk_timestamp(&__tic);
  std::sort(x.begin(), x.end());
  fclk_timestamp(&__toc);
  double elap_qsort = fclk_delta_timestamps(&__tic, &__toc);
  std::cout << "std::sort() took " << elap_qsort * 1.0e6 << " us" << std::endl;

  int numerr = 0;

  /* Finally check elementwise equivalence of the sorted results (in the expected sense)*/
  for (int i = 0; i < k; i++) {
    ksmall.PeekMinValue(&tmp);
    if (x[i] != tmp) {
      std::cout << "sorting error at position " << i << " (ksmall)" << std::endl;
      numerr++;
    }
    ksmall.RemoveMin();
    if (k <= kmaxshow) {
      std::cout << "sorted x[" << i << "] = " << x[i] << " and ksmall-min-" << i << " = " << tmp << std::endl;
    }
  }

  for (int i = 0; i < k; i++) {
    klarge.PeekMaxValue(&tmp);
    int j = n - i - 1;
    if (x[j] != tmp) {
      std::cout << "sorting error at position " << j << " (klarge)" << std::endl;
      numerr++;
    }
    klarge.RemoveMax();
    if (k <= kmaxshow) {
      std::cout << "sorted x[" << j << "] = " << x[j] << " and klarge-max-" << i << " = " << tmp << std::endl;
    }
  }

  if (numerr == 0) {
    std::cout << "*** All element checks passed ***" << std::endl;
  }

  return 0;
}
