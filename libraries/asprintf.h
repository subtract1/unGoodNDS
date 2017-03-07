// asprintf is a handy extension that sadly doesn't exist everywhere.
// Fortunately there are a bajillion PD implementations floating around.
// I simply chose one that looked reasonable and dropped it in.

#ifndef _ASPRINTF_H
#define _ASPRINTF_H

#ifndef HAVE_VASPRINTF
int vasprintf(char **str, const char *fmt, va_list ap);
#endif /*HAVE_VASPRINTF*/

#ifndef HAVE_ASPRINTF
int asprintf(char **str, const char *fmt, ...);
#endif /*HAVE_ASPRINTF*/

#endif
