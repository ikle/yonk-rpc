/*
 * Colibri RPC client helpers
 *
 * Copyright (c) 2018-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef COLIBRI_RPC_CLIENT_H
#define COLIBRI_RPC_CLIENT_H  1

#include <colibri/data/json.h>
#include <colibri/rpc/format.h>

struct json *rpc_auth (const char *key, const char *user, const char *pass);

struct json *rpc_req (const char *key, const char *mode, const char *cmd,
		      int argc, char *argv[]);

int          rpc_ans_code   (struct json *o);
const char  *rpc_ans_reason (struct json *o);
int          rpc_ans_format (struct json *o);
struct json *rpc_ans_data   (struct json *o);
int          rpc_ans_last   (struct json *o);

#endif  /* COLIBRI_RPC_CLIENT_H */
