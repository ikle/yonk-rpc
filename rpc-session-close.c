/*
 * Yonk RPC Session Close
 *
 * Copyright (c) 2018-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <unistd.h>

#include <yonk/rpc-session.h>

void rpc_session_close (struct rpc_session *o)
{
	if (o == NULL)
		return;

	close (o->socket);
	free (o);
}
