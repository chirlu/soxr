/* SoX Resampler Library      Copyright (c) 2007-16 robs@users.sourceforge.net
 * Licence for this file: LGPL v2.1                  See LICENCE for details. */

#include <soxr.h>
#include "rint.h"
#include "../examples/examples-common.h"

#define k 1000
#if defined _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #define timerDecl LARGE_INTEGER start, stop, tmp
  #define timerStart(msecs) QueryPerformanceCounter(&start), \
      QueryPerformanceFrequency(&tmp), \
      stop.QuadPart = (msecs * tmp.QuadPart + k/2) / k
  #define timerRunning() (QueryPerformanceCounter(&tmp), \
      (tmp.QuadPart-start.QuadPart < stop.QuadPart))
#else
  #include <time.h>
  #define timerDecl struct timespec stop, tmp
  #define timerStart(msecs) clock_gettime(CLOCK_MONOTONIC, &stop), \
      stop.tv_nsec += (msecs%k)*(k*k), \
      stop.tv_sec  += msecs/k + stop.tv_nsec/(k*k*k), \
      stop.tv_nsec %= k*k*k
  #define timerRunning() (clock_gettime(CLOCK_MONOTONIC, &tmp), \
      (tmp.tv_sec < stop.tv_sec || tmp.tv_nsec < stop.tv_nsec))
#endif

int main(int n, char const * arg[])
{
  char const *     const arg0 = n? --n, *arg++ : "", * engine = "";
  double          const irate = n? --n, atof(*arg++) : 96000.;
  double          const orate = n? --n, atof(*arg++) : 44100.;
  unsigned        const chans = n? --n, (unsigned)atoi(*arg++) : 1;
  soxr_datatype_t const itype = n? --n, (soxr_datatype_t)atoi(*arg++) : 0;
  unsigned        const ospec = n? --n, (soxr_datatype_t)atoi(*arg++) : 0;
  unsigned long const q_recipe= n? --n, strtoul(*arg++, 0, 16) : SOXR_HQ;
  unsigned long const q_flags = n? --n, strtoul(*arg++, 0, 16) : 0;
  double   const passband_end = n? --n, atof(*arg++) : 0;
  double const stopband_begin = n? --n, atof(*arg++) : 0;
  double const phase_response = n? --n, atof(*arg++) : -1;
  int       const use_threads = n? --n, atoi(*arg++) : 1;
  soxr_datatype_t const otype = ospec & 3;

  soxr_quality_spec_t       q_spec = soxr_quality_spec(q_recipe, q_flags);
  soxr_io_spec_t            io_spec = soxr_io_spec(itype, otype);
  soxr_runtime_spec_t const runtime_spec = soxr_runtime_spec(!use_threads);

  /* Allocate resampling input and output buffers in proportion to the input
   * and output rates: */
  #define buf_total_len 15000  /* In samples per channel. */
  size_t const osize = soxr_datatype_size(otype) * chans;
  size_t const isize = soxr_datatype_size(itype) * chans;
  size_t const olen0= (size_t)(orate * buf_total_len / (irate + orate) + .5);
  size_t const olen = min(max(olen0, 1), buf_total_len - 1);
  size_t const ilen = buf_total_len - olen;
  void * const obuf = malloc(osize * olen);
  void * const ibuf = malloc(isize * ilen);

  size_t odone = 0, clips = 0, omax = 0, i;
  soxr_error_t error;
  soxr_t soxr;


  /* Overrides (if given): */
  if (passband_end   > 0) q_spec.passband_end   = passband_end / 100;
  if (stopband_begin > 0) q_spec.stopband_begin = stopband_begin / 100;
  if (phase_response >=0) q_spec.phase_response = phase_response;
  io_spec.flags = ospec & ~7u;

  /* Create a stream resampler: */
  soxr = soxr_create(
      irate, orate, chans,         /* Input rate, output rate, # of channels. */
      &error,                         /* To report any error during creation. */
      &io_spec, &q_spec, &runtime_spec);

  if (!error) {                         /* If all is well, run the resampler: */
    engine = soxr_engine(soxr);
#define RAND ((rand()*(1./RAND_MAX)-.5)*1)
    switch (itype & 3) {
      case 0: for (i=0;i<ilen*chans; ((float   *)ibuf)[i]=(float  )RAND, ++i); break;
      case 1: for (i=0;i<ilen*chans; ((double  *)ibuf)[i]=(double )RAND, ++i); break;
      case 2: for (i=0;i<ilen*chans; ((int32_t *)ibuf)[i]=rint32(65536.*32768*RAND), ++i); break;
      case 3: for (i=0;i<ilen*chans; ((int16_t *)ibuf)[i]=rint16(    1.*32768*RAND), ++i); break;
    }
                                                       /* Resample in blocks: */
    for (i=0; i<8; ++i) {
      size_t itotal = 0, ototal = 0;
      timerDecl;
#define MSECS 125
      timerStart(MSECS);
      do {
        size_t const ilen1 = odone < olen? ilen : 0;
        error = soxr_process(soxr, ibuf, ilen1, NULL, obuf, olen, &odone);
        itotal += ilen1;
        ototal += odone;
      } while (!error && timerRunning());
      omax = max(omax, ototal);
    }
  }
                                                                  /* Tidy up: */
  clips = *soxr_num_clips(soxr);     /* Can occur only with integer output. */
  soxr_delete(soxr);
  free(obuf), free(ibuf);
                                                              /* Diagnostics: */
  fprintf(stderr, "%-26s %s; %lu clips; I/O: %s (%s) %.2fMs/s\n",
      arg0, soxr_strerror(error), (long unsigned)clips,
      ferror(stdin) || ferror(stdout)? strerror(errno) : "no error", engine,
      1e-6*k/MSECS*chans*(double)omax);
  return !!error;
}
