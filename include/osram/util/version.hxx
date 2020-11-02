#ifndef OSRAM_UTIL_VERSION
#define OSRAM_UTIL_VERSION

#define OSRAM_VERSION_MAJOR 4
#define OSRAM_VERSION_MINOR 0
#define OSRAM_VERSION_PATCH 0

#define OSRAM_VERSION__(A, B, C) "v" #A "." #B "." #C
#define OSRAM_VERSION_(A, B, C) OSRAM_VERSION__(A, B, C)
#define OSRAM_VERSION                                                          \
  OSRAM_VERSION_(OSRAM_VERSION_MAJOR, OSRAM_VERSION_MINOR, OSRAM_VERSION_PATCH)

#endif
