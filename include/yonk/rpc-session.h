/*
 * Yonk RPC Session
 *
 * Copyright (c) 2018-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef YONK_RPC_SESSION_H
#define YONK_RPC_SESSION_H  1

#include <stddef.h>
#include <sys/socket.h>

struct rpc_session {
	int socket;
	char token[37];
};

int rpc_session_create (const char *user, char *token);

struct rpc_session *rpc_session_open (const char *token);
void rpc_session_close (struct rpc_session *o);

static inline
ssize_t rpc_session_send (struct rpc_session *o, const void *data, size_t len)
{
	return send (o->socket, data, len, MSG_NOSIGNAL);
}

static inline
ssize_t rpc_session_recv (struct rpc_session *o, void *data, size_t len)
{
	return recv (o->socket, data, len, 0);
}

#endif  /* YONK_RPC_SESSION_H */
