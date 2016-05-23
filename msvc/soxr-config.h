/* SoX Resampler Library      Copyright (c) 2007-16 robs@users.sourceforge.net
 * Licence for this file: LGPL v2.1                  See LICENCE for details. */

/* N.B. Pre-configured for typical MS-Windows systems.  However, the normal
 * procedure is to use the cmake configuration and build system. See INSTALL. */

#if !defined soxr_config_included
#define soxr_config_included

#define WITH_CR32 1
#define WITH_CR32S 1
#define WITH_CR64 1
#define WITH_CR64S 1
#define WITH_VR32 1

#define AVCODEC_FOUND 0
#define AVUTIL_FOUND 0

#define HAVE_FENV_H 0
#define HAVE_STDBOOL_H 0
#define HAVE_STDINT_H 0
#define HAVE_LRINT 0
#define HAVE_BIGENDIAN 0
#define WITH_DEV_TRACE 0

#endif
