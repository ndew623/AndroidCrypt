/*
 *  intel_intrinsics.h
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file will check to see if the platform might support Intel
 *      Intrinsics and set the TERRA_USE_INTEL_INTRINSICS if so, while also
 *      including the Intel Intrinsics header file.  If one wants to disable
 *      use of Intel Intrinsics, turn off TERRA_ENABLE_INTEL_INTRINSICS.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#ifdef TERRA_ENABLE_INTEL_INTRINSICS

#if defined(__AES__) || defined(__AESNI__) || defined(__i386__) || \
    defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)

#ifndef TERRA_USE_INTEL_INTRINSICS
#define TERRA_USE_INTEL_INTRINSICS 1
#endif

#include <immintrin.h>

#endif // CPU definitions

#endif // TERRA_ENABLE_INTEL_INTRINSICS
