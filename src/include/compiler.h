#ifndef COMPILER_H
#define COMPILER_H

/*
 * Doxygen can't cope with some of the more esoteric areas of C, so we
 * make its life simpler.
 *
 */
#ifdef DOXYGEN
#define __attribute__(...)
#endif

/*
 * We export the symbol obj_OBJECT (OBJECT is defined on command-line)
 * as a global symbol, so that the linker can drag in selected object
 * files from the library using -u obj_OBJECT.
 *
 * Not quite sure why cpp requires two levels of macro call in order
 * to actually expand OBJECT...
 */
#undef _H1
#define _H1( x, y ) x ## y
#undef _H2
#define _H2( x, y ) _H1 ( x, y )
#define PREFIX_OBJECT(prefix) _H2 ( prefix, OBJECT )
#define OBJECT_SYMBOL PREFIX_OBJECT(obj_)
#undef _STR
#define _STR(s) #s
#undef _XSTR
#define _XSTR(s) _STR(s)
#define OBJECT_SYMBOL_STR _XSTR ( OBJECT_SYMBOL )

#ifdef ASSEMBLY

	.globl	OBJECT_SYMBOL
	.equ	OBJECT_SYMBOL, 0

#else /* ASSEMBLY */

__asm__ ( ".globl\t" OBJECT_SYMBOL_STR );
__asm__ ( ".equ\t" OBJECT_SYMBOL_STR ", 0" );

/*
 * Macro to allow objects to explicitly drag in other objects by
 * object name.  Used by config.c.
 *
 */
#define REQUIRE_OBJECT(object) \
	__asm__ ( ".equ\tneed_" #object ", obj_" #object );

/*
 * If debug_OBJECT is set to a true value, the macro DBG(...) will
 * expand to printf(...) when compiling OBJECT, and the symbol
 * DEBUG_LEVEL will be inserted into the object file.
 *
 */
#define DEBUG_SYMBOL PREFIX_OBJECT(debug_)

#if DEBUG_SYMBOL
#include "console.h"
#define DEBUG_SYMBOL_STR _XSTR ( DEBUG_SYMBOL )
__asm__ ( ".equ\tDEBUG_LEVEL, " DEBUG_SYMBOL_STR );
#endif

#define DBG_PRINT(...) printf ( __VA_ARGS__ )
#define DBG_DISCARD(...)
#define DBG  DBG_DISCARD
#define DBG2 DBG_DISCARD

#if DEBUG_SYMBOL >= 1
#undef DBG
#define DBG DBG_PRINT
#endif

#if DEBUG_SYMBOL >= 2
#undef DBG2
#define DBG2 DBG_PRINT
#endif

/*
 * ASSERT() macros
 *
 */
#define ASSERT(x)

#if DEBUG_SYMBOL >= 1
#undef ASSERT
#define ASSERT(x) 							      \
	do { 								      \
		if ( ! (x) ) { 						      \
			DBG ( "ASSERT(%s) failed at %s line %d [%s]\n", #x,   \
			      __FILE__, __LINE__, __FUNCTION__ );	      \
		} 							      \
	} while (0)
#endif

/*
 * Commonly-used attributes.
 *
 * Note that __used can be used only for functions.  If you have a
 * static variable declaration that you want to force to be included,
 * use __unused.
 *
 */
#define PACKED __attribute__ (( packed ))
#define __unused __attribute__ (( unused ))
#define __used __attribute__ (( used ))
#define __aligned __attribute__ (( aligned ( 16 ) ))

/*
 * To save space in the binary when multiple-driver images are
 * compiled, uninitialised data areas can be shared between drivers.
 * This will typically be used to share statically-allocated receive
 * and transmit buffers between drivers.
 *
 * Use as e.g.
 *
 *  struct {
 *	char	rx_buf[NUM_RX_BUF][RX_BUF_SIZE];
 *	char	tx_buf[TX_BUF_SIZE];
 *  } my_static_data __shared;
 *
 */

#define __shared __asm__ ( "_shared_bss" )

#endif /* ASSEMBLY */

#endif /* COMPILER_H */
