/*
 * Experimenting with _Generic() for parametric constness
 *
 * Written by Tony Finch <dot@dotat.at>
 * You may do anything with this. It has no warranty.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 */

#include <string.h>

#ifdef strchr
#undef strchr
#endif

#define strchr(s,c) _Generic((s),			\
		const char * : (const char *)		\
				(strchr)((s), (c)),	\
		char * : (strchr)((s), (c)))
