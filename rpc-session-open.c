/*
 * Yonk RPC Session Open
 *
 * Copyright (c) 2018-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <unistd.h>

#include <yonk/rpc-session.h>

struct rpc_session *rpc_session_open (const char *token)
{
	struct rpc_session *o;
	struct sockaddr_un sa;

	if ((o = malloc (sizeof (*o))) == NULL)
		return o;

	if ((o->socket = socket (AF_UNIX, SOCK_SEQPACKET, 0)) == -1)
		goto no_socket;

	snprintf (o->token, sizeof (o->token), "%s", token);

	sa.sun_family = AF_UNIX;
	memset (sa.sun_path, 0, sizeof (sa.sun_path));
	sprintf (sa.sun_path + 1, "yonk-rpc-%s", o->token);

	if (connect (o->socket, (const void *) &sa, sizeof (sa)) == -1)
		goto no_connect;

	return o;
no_connect:
	close (o->socket);
no_socket:
	free (o);
	return NULL;
}
