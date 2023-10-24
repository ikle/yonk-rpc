/*
 * Yonk RPC FastCGI wrappers and helpers
 *
 * Copyright (c) 2018-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef YONK_RPC_FCGI_LITE_H
#define YONK_RPC_FCGI_LITE_H  1

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <fcgiapp.h>

typedef FCGX_Request fcgi_req;

static inline int fcgi_init (void)
{
	return FCGX_Init ();
}

static inline int fcgi_req_init (fcgi_req *o)
{
	return FCGX_InitRequest (o, 0, 0);
}

static inline int fcgi_accept (fcgi_req *o)
{
	return FCGX_Accept_r (o);
}

static inline const char *fcgi_getenv (fcgi_req *o, const char *name)
{
	return FCGX_GetParam (name, o->envp);
}

static inline int fcgi_read (fcgi_req *o, void *data, size_t size)
{
	/* assert (size <= INT_MAX) */

	return FCGX_GetStr (data, size, o->in);
}

static inline int fcgi_write (fcgi_req *o, const void *data, size_t size)
{
	/* assert (size <= INT_MAX) */

	return FCGX_PutStr (data, size, o->out);
}

static inline int fcgi_puts (fcgi_req *o, const char *s)
{
	return FCGX_PutS (s, o->out);
}

static inline int fcgi_printf (fcgi_req *o, const char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start (ap, fmt);
	ret = FCGX_VFPrintF (o->out, fmt, ap);
	va_end (ap);

	return ret;
}

static inline int fcgi_getlen (fcgi_req *o)
{
	const char *v;

	if ((v = fcgi_getenv (o, "REQUEST_METHOD")) == NULL ||
	    strcmp (v, "POST") != 0 ||
	    (v = fcgi_getenv (o, "CONTENT_LENGTH")) == NULL)
		return 0;

	return atoi (v);
}

static inline int fcgi_pull (fcgi_req *o, void *data, size_t size)
{
	int len = fcgi_getlen (o);
	char *buf = data;

	/* assert (size <= INT_MAX) */

	if (len <= 0 || (len + 1) > size || fcgi_read (o, data, len) != len)
		return 0;

	buf[len] = '\0';
	return len;
}

#endif  /* YONK_RPC_FCGI_LITE_H */
