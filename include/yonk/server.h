/*
 * Colibri RPC server helpers
 *
 * Copyright (c) 2018-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef COLIBRI_RPC_SERVER_H
#define COLIBRI_RPC_SERVER_H  1

#include <colibri/data/json.h>
#include <colibri/rpc/format.h>

struct json *rpc_ans (int code, const char *reason, const char *data, int last);
struct json *rpc_ans_add (struct json *o, int format);

const char  *rpc_req_key  (struct json *o);
const char  *rpc_req_mode (struct json *o);
const char  *rpc_req_cmd  (struct json *o);
struct json *rpc_req_args (struct json *o);

#endif  /* COLIBRI_RPC_SERVER_H */
