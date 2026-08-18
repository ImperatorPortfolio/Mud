#ifndef ZEROPOINT_CORE_BASE_H
#define ZEROPOINT_CORE_BASE_H


#include <stdlib.h>
#include <limits.h>
#include <sys/cdefs.h>
#include <sys/time.h>

using namespace std;

#define CODENAME "ZeroPoint"
#define CODEVERSION "1.4.4"

// Backward compatibility for snippets and such.
#define mudstrlcpy strlcpy
#define mudstrlcat strlcat
#define str_dup strdup

typedef int ch_ret;
typedef int obj_ret;

#define args( list )			list

#ifdef __cplusplus
#define DECLARE_DO_FUN( fun )    extern "C" { DO_FUN    fun; } DO_FUN fun##_mangled
#define DECLARE_SPEC_FUN( fun )  extern "C" { SPEC_FUN  fun; } SPEC_FUN fun##_mangled
#define DECLARE_SPELL_FUN( fun ) extern "C" { SPELL_FUN fun; } SPELL_FUN fun##_mangled
#else
#define DECLARE_DO_FUN( fun )     DO_FUN    fun; DO_FUN fun##_mangled
#define DECLARE_SPEC_FUN( fun )   SPEC_FUN  fun; SPEC_FUN fun##_mangled
#define DECLARE_SPELL_FUN( fun )  SPELL_FUN fun; SPELL_FUN fun##_mangled
#endif

/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types. [2025 here - this hasn't been an issue for 20 years now]
 *
 * Left the definitions in for backward compatibility to old code. - Samson 6/27/07
 */
const bool TRUE = true;
const bool FALSE = false;
const short BERR = 255;

#define KEY( literal, field, value )   \
   if ( !str_cmp( word, (literal) ) )     \
{                                      \
   (field) = (value);                  \
   fMatch = TRUE;                      \
   break;                              \
}

/* Macro taken from DOTD codebase. Fcloses a file, then nulls its pointer for safety. */
#define FCLOSE(fp)  fclose((fp)); (fp)=NULL;

/*
* Structure types.
*/

#endif /* ZEROPOINT_CORE_BASE_H */
