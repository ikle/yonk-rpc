/*
 * Yonk RPC Session Open
 *
 * Copyright (c) 2018-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <syslog.h>
#include <unistd.h>

#include <uuid.h>
#include <yonk/rpc-session.h>

int rpc_session_create (const char *user, char *token)
{
	struct sockaddr_un sa;
	uuid_t uuid;
	pid_t pid;
	int s;

	if ((s = socket (AF_UNIX, SOCK_SEQPACKET, 0)) == -1)
		return 0;

	uuid_generate_random (uuid);
	uuid_unparse (uuid, token);

	sa.sun_family = AF_UNIX;
	memset (sa.sun_path, 0, sizeof (sa.sun_path));
	sprintf (sa.sun_path + 1, "yonk-rpc-%s", token);

	if (bind (s, (const void *) &sa, sizeof (sa)) == -1) {
		close (s);
		return 0;
	}

	if ((pid = fork ()) != 0) {
		close (s);
		return pid != -1;
	}

	openlog ("yonk-rpc", 0, LOG_DAEMON);

	if (dup2 (s, 0) != -1 && dup2 (s, 1) != -1) {
		if (s > 1)
			close (s);

		execlp ("su", "su", "-s", "/usr/lib/yonk-rpc/process", "-",
			user, NULL);
	}

	syslog (LOG_ERR, "Cannot start processor: %s", strerror (errno));
	closelog ();
	_exit (1);
}
