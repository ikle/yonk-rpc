/*
 * Yonk PAM login helper
 *
 * Copyright (c) 2018-2023 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef YONK_PAM_H
#define YONK_PAM_H  1

#include <security/pam_appl.h>

int pam_login (const char *group, const char *user, const char *pass);

#endif  /* COLIBRI_SYS_PAM_H */
