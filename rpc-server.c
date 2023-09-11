/*
 * Yonk RPC Server helpers
 *
 * Copyright (c) 2018-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <yonk/rpc-server.h>

struct json *rpc_ans (int code, const char *reason, const char *data, int last)
{
	struct json *o, *e;
	int ok = 1;

	if ((o = json_alloc_dict ()) == NULL)
		return NULL;

	if (code != 0) {
		if ((e = json_dict_add_list (o, "error")) == NULL)
			goto error;

		ok &= json_list_add_int    (e, code);
		ok &= json_list_add_string (e, reason);
	}

	if (data != NULL)
		ok &= json_dict_add_string (o, "data", data);

	if (!last)
		ok &= json_dict_add_bool (o, "last", false);

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
		if (!json_dict_add_string (o, "format", "text"))
			return NULL;

		return json_dict_add_list (o, "data");

	case RPC_FORMAT_DICT:
		if (!json_dict_add_string (o, "format", "dict"))
			return NULL;

		return json_dict_add_dict (o, "data");

	case RPC_FORMAT_TREE:
		if (!json_dict_add_string (o, "format", "tree"))
			return NULL;

		return json_dict_add_dict (o, "data");

	case RPC_FORMAT_TABLE:
		if (!json_dict_add_string (o, "format", "table"))
			return NULL;

		return json_dict_add_list (o, "data");
	}

	return NULL;
}

const char *rpc_req_key (struct json *o)
{
	if ((o = json_dict_lookup (o, "key")) == NULL)
		return NULL;

	return json_get_string (o);
}

const char *rpc_req_mode (struct json *o)
{
	if ((o = json_dict_lookup (o, "mode")) == NULL)
		return NULL;

	return json_get_string (o);
}

const char *rpc_req_cmd (struct json *o)
{
	if ((o = json_dict_lookup (o, "cmd")) == NULL)
		return NULL;

	return json_get_string (o);
}

struct json *rpc_req_args (struct json *o)
{
	return json_dict_lookup (o, "args");
}
