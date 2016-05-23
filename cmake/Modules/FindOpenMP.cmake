# SoX Resampler Library       Copyright (c) 2007-16 robs@users.sourceforge.net
# Licence for this file: LGPL v2.1                  See LICENCE for details.

# - Finds OpenMP support
#
# The following variables are set:
#   OpenMP_C_FLAGS - flags to add to the C compiler for this package.
#   OpenMP_FOUND - true if support for this package is found.

if (DEFINED OpenMP_C_FLAGS)
  set (TRIAL_C_FLAGS)
else ()
  set (TRIAL_C_FLAGS
    "-fopenmp"          # Gnu
    "-fopenmp=libiomp5" # Clang
    "/openmp"           # MSVC
    " "
  )

  set (TEST_C_SOURCE "
    #ifndef _OPENMP
      #error
    #endif
    #include <omp.h>
    int main() {return 0;}
  ")
endif ()

include (FindCFlags)

FindCFlags ("OpenMP" "OpenMP threading"
  "${TRIAL_C_FLAGS}" "${TEST_C_SOURCE}")

if (MINGW)
  set (OpenMP_SHARED_LINKER_FLAGS "${OpenMP_SHARED_LINKER_FLAGS} ${OpenMP_C_FLAGS}")
  set (OpenMP_EXE_LINKER_FLAGS "${OpenMP_EXE_LINKER_FLAGS} ${OpenMP_C_FLAGS}")
  mark_as_advanced (OpenMP_SHARED_LINKER_FLAGS OpenMP_EXE_LINKER_FLAGS)
endif ()
