/*
 * Colibri RPC answer format
 *
 * Copyright (c) 2018-2021 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef COLIBRI_RPC_FORMAT_H
#define COLIBRI_RPC_FORMAT_H  1

enum rpc_format {
	RPC_FORMAT_PLAIN	= 0,
	RPC_FORMAT_TEXT		= 1,
	RPC_FORMAT_DICT		= 2,
	RPC_FORMAT_TREE		= 3,
	RPC_FORMAT_TABLE	= 4,
};

#endif  /* COLIBRI_RPC_FORMAT_H */
