# SoX Resampler Library       Copyright (c) 2007-16 robs@users.sourceforge.net
# Licence for this file: LGPL v2.1                  See LICENCE for details.

# - Find AVUTIL
# Find the native installation of this package: includes and libraries.
#
#  AVUTIL_INCLUDES    - where to find headers for this package.
#  AVUTIL_LIBRARIES   - List of libraries when using this package.
#  AVUTIL_FOUND       - True if this package can be found.

if (AVUTIL_INCLUDES)
  set (AVUTIL_FIND_QUIETLY TRUE)
endif (AVUTIL_INCLUDES)

find_path (AVUTIL_INCLUDES libavutil/cpu.h)

find_library (AVUTIL_LIBRARIES NAMES avutil)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (
  AVUTIL DEFAULT_MSG AVUTIL_LIBRARIES AVUTIL_INCLUDES)

mark_as_advanced (AVUTIL_LIBRARIES AVUTIL_INCLUDES)
