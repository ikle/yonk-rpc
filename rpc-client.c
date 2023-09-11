/*
 * Colibri RPC client helpers
 *
 * Copyright (c) 2018-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>

#include <colibri/rpc/client.h>

struct json *rpc_req (const char *key, const char *mode, const char *cmd,
		      int argc, char *argv[])
{
	struct json *o, *args;
	int i, ok = 1;

	if ((o = json_dict ()) == NULL)
		return NULL;

	if (key != NULL)
		ok &= json_dict_set_string (o, "key", key);

	ok &= json_dict_set_string (o, "mode", mode);
	ok &= json_dict_set_string (o, "cmd",  cmd);

	if (argc > 0) {
		if ((args = json_dict_set_list (o, "args", argc)) == NULL)
			goto error;

		for (i = 0; i < argc; ++i)
			ok &= json_list_set_string (args, i, argv[i]);
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

	if ((auth = json_dict_set_dict (o, "args")) == NULL)
		goto error;

	ok &= json_dict_set_string (auth, "type",     "ba");
	ok &= json_dict_set_string (auth, "user",     user);
	ok &= json_dict_set_string (auth, "password", pass);

	if (ok)
		return o;
error:
	json_put (o);
	return NULL;
}

int rpc_ans_code (struct json *o)
{
	struct json *e;

	if (json_type (o) != JSON_DICT)
		return 0;

	if ((e = json_dict_get_list (o, "error")) == NULL)
		return 0;

	return json_list_get_int (e, 0);
}

const char *rpc_ans_reason (struct json *o)
{
	struct json *e;

	if (json_type (o) != JSON_DICT)
		return NULL;

	if ((e = json_dict_get_list (o, "error")) == NULL)
		return NULL;

	return json_list_get_string (e, 1);
}

int rpc_ans_format (struct json *o)
{
	const char *format;

	if (json_type (o) != JSON_DICT)
		return RPC_FORMAT_PLAIN;

	if ((format = json_dict_get_string (o, "format")) == NULL)
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
	if (json_type (o) != JSON_DICT)
		return NULL;

	return json_dict_get (o, "data");
}

int rpc_ans_last (struct json *o)
{
	struct json *last;

	if (json_type (o) != JSON_DICT)
		return 1;

	if ((last = json_dict_get (o, "last")) == NULL)
		return 1;

	return json_get_bool (last);
}
