/*
 * Yonk RPC Client helpers
 *
 * Copyright (c) 2018-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>

#include <yonk/rpc-client.h>

struct json *rpc_req (const char *key, const char *mode, const char *cmd,
		      int argc, char *argv[])
{
	struct json *o, *args;
	int i, ok = 1;

	if ((o = json_alloc_dict ()) == NULL)
		return NULL;

	if (key != NULL)
		ok &= json_dict_add_string (o, "key", key);

	ok &= json_dict_add_string (o, "mode", mode);
	ok &= json_dict_add_string (o, "cmd",  cmd);

	if (argc > 0) {
		if ((args = json_dict_add_list (o, "args")) == NULL)
			goto error;

		for (i = 0; i < argc; ++i)
			ok &= json_list_add_string (args, argv[i]);
	}

	if (ok)
		return o;
error:
	json_put (o);
	return NULL;
}

struct json *rpc_auth (const char *key, const char *user, const char *pass)
{
	struct json *o, *auth;
	int ok = 1;

	if ((o = rpc_req (key, "session", "login", 0, NULL)) == NULL)
		return NULL;

	if ((auth = json_dict_add_dict (o, "args")) == NULL)
		goto error;

	ok &= json_dict_add_string (auth, "type",     "ba");
	ok &= json_dict_add_string (auth, "user",     user);
	ok &= json_dict_add_string (auth, "password", pass);

	if (ok)
		return o;
error:
	json_put (o);
	return NULL;
}

int rpc_ans_code (struct json *o)
{
	if ((o = json_dict_lookup (o, "error")) == NULL ||
	    (o = json_list_lookup (o, 0)) == NULL)
		return 0;

	return json_get_int (o);
}

const char *rpc_ans_reason (struct json *o)
{
	if ((o = json_dict_lookup (o, "error")) == NULL ||
	    (o = json_list_lookup (o, 1)) == NULL)
		return NULL;

	return json_get_string (o);
}

int rpc_ans_format (struct json *o)
{
	const char *format;

	if ((o = json_dict_lookup (o, "format")) ||
	    (format = json_get_string (o)) == NULL)
		return RPC_FORMAT_PLAIN;

	if (strcmp (format, "text") == 0)
		return RPC_FORMAT_TEXT;

	if (strcmp (format, "dict") == 0)
		return RPC_FORMAT_DICT;

	if (strcmp (format, "tree") == 0)
		return RPC_FORMAT_TREE;

	if (strcmp (format, "table") == 0)
		return RPC_FORMAT_TABLE;

	return RPC_FORMAT_PLAIN;
}

struct json *rpc_ans_data (struct json *o)
{
	return json_dict_lookup (o, "data");
}

bool rpc_ans_last (struct json *o)
{
	if ((o = json_dict_lookup (o, "last")) == NULL)
		return true;

	return json_get_bool (o);
}
