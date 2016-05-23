/* SoX Resampler Library      Copyright (c) 2007-16 robs@users.sourceforge.net
 * Licence for this file: LGPL v2.1                  See LICENCE for details. */

#include "soxr.h"
#include "filter.h"
#include "internal.h"
#include <math.h>
#include <stdarg.h>
#include <string.h>

#if !WITH_CR32 && !WITH_CR32S && !WITH_CR64 && !WITH_CR64S
#undef lsx_to_3dB
#define lsx_to_3dB(x) ((x)/(x))
#endif



soxr_quality_spec_t soxr_quality_spec(unsigned long recipe, unsigned long flags)
{
  soxr_quality_spec_t spec, * p = &spec;
  unsigned quality = recipe & 0xf;
  double rej;
  memset(p, 0, sizeof(*p));
  if (quality > SOXR_PRECISIONQ) {
    p->e = "invalid quality type";
    return spec;
  }
  flags |= quality < SOXR_LSR0Q ? RESET_ON_CLEAR : 0;
  p->phase_response = "\62\31\144"[(recipe & 0x30)>>4];
  p->stopband_begin = 1;
  p->precision =
    quality == SOXR_QQ      ?  0 :
    quality <= SOXR_16_BITQ ? 16 :
    quality <= SOXR_32_BITQ ?  4 + quality * 4 :
    quality <= SOXR_LSR2Q   ? 55 - quality * 4 : /* TODO: move to lsr.c */
    0;
  rej = p->precision * linear_to_dB(2.);
  p->flags = flags;
  if (quality <= SOXR_32_BITQ || quality == SOXR_PRECISIONQ) {
    #define LOW_Q_BW0     (1385 / 2048.) /* 0.67625 rounded to be a FP exact. */
    p->passband_end = quality == 1? LOW_Q_BW0 : 1 - .05 / lsx_to_3dB(rej);
    if (quality <= 2)
      p->flags &= ~SOXR_ROLLOFF_NONE, p->flags |= SOXR_ROLLOFF_MEDIUM;
  }
  else { /* TODO: move to lsr.c */
    static float const bw[] = {.931f, .832f, .663f};
    p->passband_end = bw[quality - SOXR_LSR0Q];
    if (quality == SOXR_LSR2Q)
      p->flags &= ~SOXR_ROLLOFF_NONE, p->flags |= SOXR_ROLLOFF_LSR2Q | SOXR_PROMOTE_TO_LQ;
  }
  if (recipe & SOXR_STEEP_FILTER)
    p->passband_end = 1 - .01 / lsx_to_3dB(rej);
  return spec;
}



soxr_runtime_spec_t soxr_runtime_spec(unsigned num_threads)
{
  soxr_runtime_spec_t spec, * p = &spec;
  memset(p, 0, sizeof(*p));
  p->log2_min_dft_size = 10;
  p->log2_large_dft_size = 17;
  p->coef_size_kbytes = 400;
  p->num_threads = num_threads;
  return spec;
}



soxr_io_spec_t soxr_io_spec(
  soxr_datatype_t itype,
  soxr_datatype_t otype)
{
  soxr_io_spec_t spec, * p = &spec;
  memset(p, 0, sizeof(*p));
  if ((itype | otype) >= SOXR_SPLIT * 2)
    p->e = "invalid io datatype(s)";
  else {
    p->itype = itype;
    p->otype = otype;
    p->scale = 1;
  }
  return spec;
}
