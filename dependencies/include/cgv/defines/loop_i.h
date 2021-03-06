/*! \file loop_i.h
	\brief Include this header to perform a preprocessor loop in the variable \c I.

	When programming with templates one often has to write code fragments that could
	be generated by a preprocessor loop more easily. For example the declaration
\code
template <typename T1,typename T2,typename T3,typename T4,...,typename T12>
\endcode
	could be written much more efficiently if there existed the <tt>\#for</tt> directive for
	the preprocessor:
\code
template <typename T1
#for (I=2;I<13;++I)
	, typename T##I
#endfor
>
\endcode
	The preprocessor library of boost has support for a similar construct and
	uses nested macros to implement it via complete loop unrolling. In this way
	for every loop iteration a different macro has to be defined leading to a large
	number of macro declarations. Loop unrolling becomes necessary as recursive 
	function macro expansion is not supported by the C preprocessor.

	The implementation of looping in this header is based on file includes that 
	can be performed recursively. The following naive implementation does not 
	work though:
\code
template <typename T1
#define I 2
#include <loop_recurse.h>
>
--------------<loop_recurse.h>--------------
#if I<13
, typename T##I
#define I I+1
#include <loop_recurse.h>
#endif
\endcode
	The line that does not work is <tt>\#define I I+1</tt>. It is extremely 
	cumbersome to force the preprocessor to evaluate the expression <tt>I+1</tt> 
	and assign the result to a new variable. The only way seem to be the \c slot
	mechanism proposed in boost/pp/slot.h, which does exactly that. With this 
	one can rewrite the loop implementation to
\code
template <typename T1
#define I 2
#include <loop_recurse.h>
>
--------------<loop_recurse.h>--------------
#if I<13
, typename T##I
#define BOOST_PP_VALUE I+1
#include BOOST_PP_ASSIGN_SLOT(1)
#undef I
#define I BOOST_PP_SLOT(1)
#include <loop_recurse.h>
#endif
\endcode
	In order to hide the implementation detail, this file provides a simple way
	to do such loops. The loop body can either be defined in a macro named
	\c LOOP_I_BODY or in an include file, whose name is specified in the macro
	\c LOOP_I_FILE. The above code simplifies to
\code
template <typename T1
#define LOOP_I_BEGIN 2
#define LOOP_I_END 13
#define LOOP_I_BODY , typename JOIN(T,I)
#include <cgv/defines/loop_i.h>
>
\endcode
	Note that all parameters like the begin value and the end value of the loop
	parameter \c I are defined via macros. After usage of loop_i.h all macros will 
	be automatically undefined. Default values can be defined in the macros
	\c CGV_DEFINES_LOOP_STD_BEGIN and \c CGV_DEFINES_LOOP_STD_END. If no default
	values are given, these are set to 0 and 8 respectively in the header 
	loop_begin.h .
	
	Note also that the ##-operator cannot be used if the loop body
	is defined via a macro. You need to use \c CGV_DEFINES_JOIN(X,Y) instead, which
	is available in loop bodies in the short form \c JOIN(X,Y).

	When using a file as loop body one can also nest several loops. For this the
	files loop_j.h and loop_k.h exist and work in the same way as loop_i.h except
	that in all names \c I is replaced by \c J or \c K. See cgv_defines_loop_tests.cxx 
	for an example, where even the current source file is reused as include file.

	For convenience one can also define the macro \c LOOP_I_SEP to define a separator sign, 
	that is inserted in between instances of the loop body. The following example
	generates <tt>sum=a0+a1+a2;</tt>:
\code
sum=
#define LOOP_I_END 3
#define LOOP_I_BODY JOIN(a,I)
#define LOOP_I_SEP +
#include <cgv/defines/loop_i.h>
;
\endcode
*/
#include <cgv/defines/loop/loop_begin.h>
/// \cond
// map local definitions to global name space
#ifdef LOOP_I_BEGIN
#	define CGV_DEFINES_LOOP_I_BEGIN LOOP_I_BEGIN
#else
#	define CGV_DEFINES_LOOP_I_BEGIN CGV_DEFINES_LOOP_STD_BEGIN
#endif
#
#ifdef LOOP_I_END
#	define CGV_DEFINES_LOOP_I_END LOOP_I_END
#else
#	define CGV_DEFINES_LOOP_I_END CGV_DEFINES_LOOP_STD_END
#endif
#ifdef LOOP_I_BODY
#	define CGV_DEFINES_LOOP_I_BODY LOOP_I_BODY
#endif
#ifdef LOOP_I_FILE
#	define CGV_DEFINES_LOOP_I_FILE LOOP_I_FILE
#endif
#ifdef LOOP_I_SEP
#	define CGV_DEFINES_LOOP_I_SEP LOOP_I_SEP
#endif
// loop initialization
#define CGV_DEFINES_EXPRESSION CGV_DEFINES_LOOP_I_BEGIN
#include CGV_DEFINES_ASSIGN_SLOT(1)
#include <cgv/defines/loop/loop_i_recurse.h>
#
#undef CGV_DEFINES_LOOP_I_END
#undef CGV_DEFINES_LOOP_I_BEGIN
#undef CGV_DEFINES_LOOP_I_BODY
#undef CGV_DEFINES_LOOP_I_FILE
#undef CGV_DEFINES_LOOP_I_SEP
#undef LOOP_I_BEGIN
#undef LOOP_I_END
#undef LOOP_I_BODY
#undef LOOP_I_FILE
#undef LOOP_I_SEP
/// \endcond
#include <cgv/defines/loop/loop_end.h>
