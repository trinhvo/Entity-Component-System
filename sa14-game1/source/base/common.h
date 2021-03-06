/*------------------------------------------------------------------------------
 * File: common.h
 * Created: June 7, 2015
 * Last changed: July 24, 2015
 *
 * Author(s): Philip Arvidsson (contact@philiparvidsson.com)
 *
 * Description:
 *   Macros, constants, types, functions etc. that are common to many parts of
 *   the program.
 *----------------------------------------------------------------------------*/

#ifndef common_h_
#define common_h_

/*------------------------------------------------
 * CONFIGURATION
 *----------------------------------------------*/

// If we're on MSVC++ we can configure the compilation and linking processes
// here.
// @To-do: Move out to a makefile or such.
#ifdef _MSC_VER

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

// This warning is not quite relevant since anonymous structs and unions are
// allowed in the C11 standard, but some compilers are not quite there yet and
// consider it nonstandard.
//   warning: 'nonstandard extension used : nameless struct/union'
#pragma warning(disable:4201)

// This warning is not relevant. Non-constant initializers were introduced in
// the C99 standard.
//   warning: nonstandard extension used: non-constant aggregate initializer
#pragma warning(disable:4204)

// We don't need to see warnings about functions being inlined.
//   warning: 'function selected for automatic inline expansion'
#pragma warning(disable:4711)

#endif // _MSC_VER

/*------------------------------------------------
 * DEFINES
 *----------------------------------------------*/

// Uncomment below to render geometry normals.
//#define DRAW_TRI_NORMALS

/*------------------------------------------------
 * MACROS
 *----------------------------------------------*/

#ifdef _MSC_VER

// The inline keyword was introduced in C99 so MSVC++ doesn't support it.
// However, it supports the __inline keyword which exactly the same, so we can
// just alias it on MSVC++.
#define inline __inline

// The _Noreturn keyword was introduced in C11.
#define _Noreturn  __declspec(noreturn)

#endif // _MSC_VER

/*--------------------------------------
 * Macro: string
 *
 * Description:
 *   String type macro.
 *------------------------------------*/
#define string stringT

#ifndef clamp
    #define clamp(x, a, b) max(min(x, b), a)
#endif // clamp

#ifndef min
    #define min(a, b) ((a) < (b) ? (a) : (b))
#endif // min

#ifndef max
    #define max(a, b) ((a) > (b) ? (a) : (b))
#endif // max

/*------------------------------------------------
 * CONSTANTS
 *----------------------------------------------*/

#ifdef _DEBUG
    // The .pak-files are only encrypted in release mode, so specify no password
    // in debug mode.
    #define PakPassword ((void*)0)
#else
    #define PakPassword ("n3m3s1s!")
#endif // _DEBUG

/*--------------------------------------
 * Constant: ProgramAuthors
 *
 * Description:
 *   A string containing the program authors.
 *------------------------------------*/
#define ProgramAuthors ("Philip Arvidsson (contact@philiparvidsson.com)")

/*--------------------------------------
 * Constant: ProgramVersion
 *
 * Description:
 *   The program version.
 *------------------------------------*/
#define ProgramVersion ("0.07b")

#define ResBinary  (0x01)
#define ResString  (0x02)
#define ResShader  (0x04)
#define ResMesh    (0x08)
#define ResTexture (0x10)

/*------------------------------------------------
 * TYPES
 *----------------------------------------------*/

/*--------------------------------------
 * Type: stringT
 *
 * Description:
 *   String type definition.
 *------------------------------------*/
typedef char stringT;

/*------------------------------------------------
 * INCLUDES
 *----------------------------------------------*/

// Some includes might depend on definitions in this file, so we put them at the
// end of this file instead of the beginning.

#include "base/debug.h"

#include <stdbool.h>
#include <stdlib.h>

#include <wchar.h>

/*------------------------------------------------
 * FUNCTIONS
 *----------------------------------------------*/

#ifdef WIN32
void sleep(int millisecs);
#endif // WIN32

#ifdef __linux__
extern int usleep(unsigned int useconds);
#define sleep(millisecs) (usleep(millisecs*1000))
#endif // __linux__

wchar_t* wstrdup(const string* str);
int strcmpi2(const string* str1, const string* str2);

#endif // common_h_
