/*
 * Linux 2.6.32 and later Kernel module for VMware MVP Guest Communications
 *
 * Copyright (C) 2010-2013 VMware, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */
#line 5

/**
 * @file
 *
 * @brief basic type definitions.
 * These may need to be conditionalized for different compilers/platforms.
 */

#ifndef _MVPTYPES_H
#define _MVPTYPES_H

#define INCLUDE_ALLOW_MVPD
#define INCLUDE_ALLOW_VMX
#define INCLUDE_ALLOW_MODULE
#define INCLUDE_ALLOW_MONITOR
#define INCLUDE_ALLOW_PV
#define INCLUDE_ALLOW_HOSTUSER
#define INCLUDE_ALLOW_GUESTUSER
#define INCLUDE_ALLOW_WORKSTATION
#define INCLUDE_ALLOW_GPL
#include "include_check.h"

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

typedef signed char         int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;

typedef uint32 GVA;      /* guest virtual addresses */
typedef uint64 PA;       /* (guest) physical addresses (40-bit) */

#if __SIZEOF_POINTER__ == 8
typedef uint64 CVA;      /* cache virtual addresses */
typedef uint64 MVA;      /* monitor virtual addresses */
typedef uint64 HKVA;     /* host kernel virtual addresses */
typedef uint64 HUVA;     /* host user virtual addresses */
typedef uint64 MA;       /* (host) machine addresses */

typedef uint64 PPN;       /* PA/PAGE_SIZE */
typedef uint64 MPN;       /* MA/PAGE_SIZE */

#define LONG_FORMAT	"llx"
#define SIZE_T_FORMAT	"ld"
#else
typedef uint32 CVA;      /* cache virtual addresses */
typedef uint32 MVA;      /* monitor virtual addresses */
typedef uint32 HKVA;     /* host kernel virtual addresses */
typedef uint32 HUVA;     /* host user virtual addresses */
typedef uint32 MA;       /* (host) machine addresses */

typedef uint32 PPN;       /* PA/PAGE_SIZE */
typedef uint32 MPN;       /* MA/PAGE_SIZE */

#define LONG_FORMAT	"x"
#define SIZE_T_FORMAT	"d"
#endif

typedef uint64 cycle_t;

/**
 * @brief Page segment.
 *
 * Specifies a segment within a single page.
 */
typedef struct {
	uint16 off;
	uint16 len;
} PageSeg;

/*
 * GCC's argument checking for printf-like functions
 *
 * fmtPos is the position of the format string argument, beginning at 1
 * varPos is the position of the variable argument, beginning at 1
 */

#if defined(__GNUC__)
# define PRINTF_DECL(fmtPos, varPos) \
	__attribute__((__format__(__printf__, fmtPos, varPos)))
#else
# define PRINTF_DECL(fmtPos, varPos)
#endif

#if defined(__GNUC__)
# define SCANF_DECL(fmtPos, varPos) \
	__attribute__((__format__(__scanf__, fmtPos, varPos)))
#else
# define SCANF_DECL(fmtPos, varPos)
#endif

#endif /* _MVPTYPES_H */
