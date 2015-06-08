/*
 * Copyright (c) 1992-2015 T. R. Burghart.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file has been subjected to so many major and minor rewrites over
 * the years that I have no idea whether it'll break something if used
 * with some ancient codebase or compiler.
 *
 * I try not to break things gratuitously, but a lot of compiler macros
 * have changed over this file's multi-decade lifespan and I don't keep
 * crufty old compilers (or the crufty old computers that run them)
 * around for sport.
 *
 * I've removed all the old 16-bit stuff, because anything that still
 * runs in that mode should be platform specific, so it doesn't need a
 * file like this that's all about cross-platform compatibility.
 *
 * I've also pared down the recognized compilers, CPUs, and operating
 * systems drastically, because I haven't used some of them in a very long
 * time. If I have occasion to use them again, I'll put them back in when
 * I can test against a live platform that's still relevant.
 *
 * This *should* remain compatible with something aound C89 or so.
 */

/*
 * don't use "#pragma once" in this file because some compilers wrongly
 * emit a warning for an unrecognized pragma
 */
#ifndef Burghart_platform_h_included
#define Burghart_platform_h_included 1

/*
 * Work out the language support level up front, and allow a short-circuit
 * mechanism for including sources.
 *
 * COMPILER_C_STD is set to a positive long integer constant representing
 * the full year indicated by the C language standard to which the current
 * compilation conforms, such that more recent standards always have higher
 * values than older versions (e.g. C11 is higher than C99, because they are
 * translated to 2011 and 1999, respecively). Note that the year indicated
 * by the COMPILER_C_STD macro doesn't always match the common name of the
 * standard - in ambiguous cases, COMPILER_C_STD is set to the lower value
 * (e.g. C95 == 1994, C90 == 1989).
 *
 * If COMPILER_C_STD >= 1994, then the __STDC_VERSION__ macro is available to
 * obtain the exact version and its value will be >= (COMPILER_C_STD * 100).
 * If COMPILER_C_STD < 1989, then the compiler doesn't declare conformance
 * with any standard and should a) be assumed to implement some variant of
 * K&R C and b) be replaced before subjecting another single line of code to
 * its eccentricities!
 *
 * If COMPILER_C_STD_MIN is set it's compared to the supported level and an
 * error is generated if the supported level is less than the required level.
 */
#if     defined(__STDC_VERSION__)
#define COMPILER_C_STD      (__STDC_VERSION__ / 100)
#else
#if     defined(__STDC__) && (__STDC__ > 0)
#define COMPILER_C_STD      1989L
#else
#define COMPILER_C_STD      1988L
#endif
#endif
#if     defined(COMPILER_C_STD_MIN) && (COMPILER_C_STD_MIN > COMPILER_C_STD)
#error  Required minimum C standard COMPILER_C_STD_MIN not met!
#endif

/*
 * The order in which compiler identifiers are checked is important,
 * because a lot of them also define common macros identifying other
 * compilers, especially GCC and MSVC (and, increasingly, Clang).
 *
 * Refer to http://sourceforge.net/p/predef/wiki/Home/
 * for general implementation-specific macros, or each compiler's
 * documentation for specifics.
 */

/*
 * Intel C/C++ will mock a common feature set depending on platform.
 *
 * https://software.intel.com/en-us/c-compilers
 * https://software.intel.com/en-us/compiler_15.0_ug_c
 *
 */
#if     defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC)
#define COMPILER_INTEL      1
#define COMPILER_ID_NAME    "Intel"
#if     defined(__INTEL_COMPILER)
#define COMPILER_ID_VERS    __INTEL_COMPILER
#elif   defined(__ICL)
#define COMPILER_ID_VERS    __ICL
#else
#define COMPILER_ID_VERS    __ICC
#endif

/*
 * Not sure whether Comeau C/C++ is still worth keeping about, it's looking
 * kinda ill on its web site. However, it has a history of strong language
 * validation, and it mocks other compilers' platform definitions making it
 * low maintenance, so I'll keep it here for now.
 * 
 * http://www.comeaucomputing.com
 */
#elif   defined(__COMO__)
#define COMPILER_COMO       1
#define COMPILER_ID_NAME    "Comeau"
#if     defined(__EDG_VERSION__)
#define COMPILER_ID_VERS    __EDG_VERSION__
#else
#define COMPILER_ID_VERS    __COMO_VERSION__
#endif

/*
 * Sun/Oracle/Forte whatever.
 *
 * http://www.oracle.com/technetwork/server-storage/solarisstudio/documentation/index.html
 */
#elif   defined(__SUNPRO_C)
#define COMPILER_SUN        1
#define COMPILER_ID_NAME    "Sun"
#define COMPILER_ID_VERS    __SUNPRO_C

/*
 * Clang mocks GCC's features.
 *
 * http://clang.llvm.org/docs
 */
#elif   defined(__clang__)
#define COMPILER_CLANG      1
#define COMPILER_ID_NAME    "Clang"
#define COMPILER_ID_VERS    ((__clang_major__ * 100) + __clang_minor__)

/*
 * Actual GCC.
 *
 * https://gcc.gnu.org/onlinedocs/gcc/
 */
#elif   defined(__GNUC__)
#define COMPILER_GCC        1
#define COMPILER_ID_NAME    "GCC"
#define COMPILER_ID_VERS    ((__GNUC__ * 100) + __GNUC_MINOR__)

/*
 * Actual MSVC.
 *
 * https://msdn.microsoft.com/en-us/library/
 * https://msdn.microsoft.com/en-us/library/b0084kay.aspx
 */
#elif   defined(_MSC_VER)
#define COMPILER_MSVC       1
#define COMPILER_ID_NAME    "MSVC"
#define COMPILER_ID_VERS    _MSC_VER

#else
#error  Unrecognized/unsupported compiler!
#endif  /* compiler identification */

/*
 * A lot of compilers mock other compilers' behavior.
 * Some even mock more than one, so no #elif.
 */
#if     defined(__clang__) && !defined(COMPILER_CLANG)
#define COMPILER_CLANG      1
#endif
#if     defined(__GNUC__) && !defined(COMPILER_GCC)
#define COMPILER_GCC        1
#endif
#if     defined(_MSC_VER) && !defined(COMPILER_MSVC)
#define COMPILER_MSVC       1
#endif

#if     defined(__x86_64) || defined(__amd64) \
    ||  defined(_M_X64) || defined(_M_AMD64) \
    ||  defined(__x86_64__) || defined(__amd64__)
#define PLATFORM_BITS       64
#define PLATFORM_ENDIAN_L   1
#define PLATFORM_X86        1
#define PLATFORM_X86_64     1

/*
 * There are a ton more for other compilers that I'm ignoring.
 */
#elif   defined(__i386) || defined(_M_I386) || defined(__IA32__) \
    ||  (defined(_M_IX86) && !defined(__WATCOMC__)) || defined(__i386__)
#define PLATFORM_BITS       32
#define PLATFORM_ENDIAN_L   1
#define PLATFORM_X86        1
#define PLATFORM_X86_32     1

#elif   defined(__sparc) || defined(__sparc__)
#define PLATFORM_SPARC      1
#define PLATFORM_ENDIAN_B   1
#if     defined(__sparcv9) || defined(__sparcv9__)
#define PLATFORM_BITS       64
#define PLATFORM_SPARC_64   1
#else
#define PLATFORM_BITS       32
#define PLATFORM_SPARC_32   1
#endif

/*
 * No longer bothering with:
 *  Alpha
 *  HP-PA
 *  Itanium
 *  PowerPC
 *  etc ...
 */
#else
#error  Unrecognized/unsupported CPU!
#endif  /* CPU identification */

#if     defined(PLATFORM_BITS)
#if     PLATFORM_BITS == 64
#define PLATFORM_64BIT      1
#elif   PLATFORM_BITS == 32
#define PLATFORM_32BIT      1
#elif   PLATFORM_BITS == 16
#define PLATFORM_16BIT      1
#endif
#endif

/*
 * if PLATFORM_UNIX > 0 then #include<unistd.h> works
 * if PLATFORM_WINDOWS > 0 then #include<windows.h> works
 */
#if     defined(__APPLE__) && defined(__MACH__)
#define PLATFORM_OSX        1
#define PLATFORM_OS_NAME    "Mac OS X"
#define PLATFORM_BSD        1
#define PLATFORM_UNIX       1

#elif   defined(__FreeBSD__)
#define PLATFORM_BSD        1
#define PLATFORM_OS_NAME    "FreeBSD"
#define PLATFORM_UNIX       1

#elif   defined(__NetBSD__)
#define PLATFORM_BSD        1
#define PLATFORM_OS_NAME    "NetBSD"
#define PLATFORM_UNIX       1

#elif   defined(__OpenBSD__)
#define PLATFORM_BSD        1
#define PLATFORM_OS_NAME    "OpenBSD"
#define PLATFORM_UNIX       1

#elif   defined(__DragonFly__)
#define PLATFORM_BSD        1
#define PLATFORM_OS_NAME    "DragonFly BSD"
#define PLATFORM_UNIX       1

#elif   defined(__linux) || defined(__linux__)
#define PLATFORM_LINUX      1
#define PLATFORM_OS_NAME    "Linux"
#define PLATFORM_UNIX       1

#elif   defined(__sun) || defined(sun)
#define PLATFORM_SUNOS      1
#define PLATFORM_UNIX       1
#if     defined(__SVR4) || defined(__svr4__)
#define PLATFORM_SOLARIS    1
#define PLATFORM_OS_NAME    "Splaris"
#define PLATFORM_SVR4       1
#else
#define PLATFORM_OS_NAME    "SunOS"
#define PLATFORM_BSD        1
#endif

#elif   defined(_WIN64) || defined(_WIN32)
#define PLATFORM_WINDOWS    1

#else
#error  Unrecognized/unsupported OS!
#endif  /* OS identification */

/*
 * Pick up any stragglers ...
 */
#if     defined(__bsdi__)
#define PLATFORM_BSD        1
#endif
#if     defined(__SVR4) || defined(__svr4__)
#define PLATFORM_SVR4       1
#endif
#if     defined(PLATFORM_BSD) || defined(PLATFORM_SVR4) \
    ||  defined(__unix) || defined(__unix__) || defined(unix)
#define PLATFORM_UNIX       1
#endif

#endif  /* Burghart_platform_h_included */
