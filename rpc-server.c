/*
 * Colibri RPC server helpers
 *
 * Copyright (c) 2018-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>

#include <colibri/rpc/server.h>

struct json *rpc_ans (int code, const char *reason, const char *data, int last)
{
	struct json *o, *e;
	int ok = 1;

	if ((o = json_dict ()) == NULL)
		return NULL;

	if (code != 0) {
		if ((e = json_dict_set_list (o, "error", 2)) == NULL)
			goto error;

		ok &= json_list_set_int    (e, 0, code);
		ok &= json_list_set_string (e, 1, reason);
	}

	if (data != NULL)
		ok &= json_dict_set_string (o, "data", data);

	if (!last)
		ok &= json_dict_set_bool (o, "last", last);

	if (ok)
		return o;
error:
	json_put (o);
	return NULL;
}

struct json *rpc_ans_add (struct json *o, int format)
{
	switch (format) {
	case RPC_FORMAT_TEXT:
		if (!json_dict_set_string (o, "format", "text"))
			return NULL;

		return json_dict_set_list (o, "data", 0);

	case RPC_FORMAT_DICT:
		if (!json_dict_set_string (o, "format", "dict"))
			return NULL;

		return json_dict_set_dict (o, "data");

	case RPC_FORMAT_TREE:
		if (!json_dict_set_string (o, "format", "tree"))
			return NULL;

		return json_dict_set_dict (o, "data");

	case RPC_FORMAT_TABLE:
		if (!json_dict_set_string (o, "format", "table"))
			return NULL;

		return json_dict_set_list (o, "data", 0);
	}

	return NULL;
}

const char *rpc_req_key (struct json *o)
{
	if (json_type (o) != JSON_DICT)
		return NULL;

	return json_dict_get_string (o, "key");
}

const char *rpc_req_mode (struct json *o)
{
	if (json_type (o) != JSON_DICT)
		return NULL;

	return json_dict_get_string (o, "mode");
}

const char *rpc_req_cmd (struct json *o)
{
	if (json_type (o) != JSON_DICT)
		return NULL;

	return json_dict_get_string (o, "cmd");
}

struct json *rpc_req_args (struct json *o)
{
	if (json_type (o) != JSON_DICT)
		return NULL;

	return json_dict_get (o, "args");
}
