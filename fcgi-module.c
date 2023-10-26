/*
 * Yonk RPC FastCGI Module
 *
 * Copyright (c) 2018-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <string.h>

#include <syslog.h>
#include <unistd.h>

#include <yonk/json-io.h>
#include <yonk/pam.h>
#include <yonk/rpc-server.h>
#include <yonk/rpc-session.h>

#include "fcgi-lite.h"

static int reply_error (fcgi_req *o, const char *object)
{
	if (object == NULL || errno == ENOENT || errno == ECONNREFUSED ||
	    errno == ECONNRESET || errno == EPIPE) {
		fcgi_puts (o, "Status: 401 Unauthorized\r\n"
			      "WWW-Authenticate: Bearer\r\n"
			      "\r\n");
		return 0;
	}

	if (errno == 0) {
		fcgi_puts (o, "Status: 400 Bad Request\r\n\r\n");
		return 0;
	}

	syslog (LOG_ERR, "%s: %s", object, strerror (errno));
	fcgi_puts (o, "Status: 500 Internal Server Error\r\n\r\n");
	return 0;
}

static int reply_ok (fcgi_req *o, const char *data, size_t len)
{
	fcgi_printf (o, "Status: 200 OK\r\n"
			"Content-Type: application/json\r\n"
			"Content-Length: %zu\r\n"
			"\r\n", len);
	fcgi_write (o, data, len);
	return 1;
}

static const char *json_dict_get_string (struct json *o, const char *key)
{
	if ((o = json_dict_lookup (o, key)) == NULL)
		return NULL;

	return json_get_string (o);
}

static int on_anon (fcgi_req *o)
{
	char data[256], token[37];
	int len;
	struct json *r, *args;
	const char *v, *user, *pass;

	if ((len = fcgi_pull (o, data, sizeof (data))) <= 0 ||
	    (r = json_read_blob (data, len)) == NULL)
		goto no_pull;

	if ((v = rpc_req_mode (r)) == NULL || strcmp (v, "session") != 0 ||
	    (v = rpc_req_cmd  (r)) == NULL || strcmp (v, "login")   != 0 ||
	    (args = rpc_req_args (r)) == NULL ||
	    (v = json_dict_get_string (args, "type")) == NULL ||
	    strcmp (v, "ba") != 0 ||
	    (user = json_dict_get_string (args, "user")) == NULL ||
	    (pass = json_dict_get_string (args, "password")) == NULL)
		goto no_login;

	if (pam_login ("yonk-rpc", user, pass) != 0)
		len = snprintf (data, sizeof (data),
				"{\"error\":[1, \"Access Denied\"]}");
	else
	if (!rpc_session_create (user, token))
		len = snprintf (data, sizeof (data),
				"{\"error\":[2, \"Internal Error\"]}");
	else
		len = snprintf (data, sizeof (data), "{\"data\":\"%s\"}",
				token);
	json_put (r);
	return reply_ok (o, data, len);
no_login:
	json_put (r);
no_pull:
	return reply_error (o, "anon");
}

static int on_sess (fcgi_req *o, const char *token)
{
	struct rpc_session *s;
	char data[1024];
	int len;

	if ((len = fcgi_pull (o, data, sizeof (data))) <= 0 ||
	    (s = rpc_session_open (token)) == NULL)
		goto no_open;

	if (rpc_session_send (s, data, len) != len ||
	    (len = rpc_session_recv (s, data, sizeof (data))) < 0)
		goto no_trade;

	rpc_session_close (s);
	return reply_ok (o, data, len);
no_trade:
	rpc_session_close (s);
no_open:
	return reply_error (o, "process");
}

static int process (fcgi_req *o)
{
	const char *https = fcgi_getenv (o, "HTTPS");
	const char *auth  = fcgi_getenv (o, "HTTP_AUTHORIZATION");

	errno = 0;

	if (https == NULL || strcmp (https, "on") != 0)
		return reply_error (o, NULL);

	if (auth == NULL || strncmp (auth, "Bearer ", 7) != 0)
		return on_anon (o);

	return on_sess (o, auth + 7);
}

int main (int argc, char *argv[])
{
	fcgi_req o;

	openlog ("yonk-rpc", 0, LOG_DAEMON);

	if (fcgi_init () != 0 || fcgi_req_init (&o) != 0) {
		syslog (LOG_ERR, "FCGI initialization failed");
		return 1;
	}

	while (fcgi_accept (&o) == 0)
		(void) process (&o);

	syslog (LOG_INFO, "FCGI stopped");
	return 0;
}
