/*
 * Novantotto
 * Copyright (c) 2024 Andrea Bonomi
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// clang-format off
#ifdef DEBUG
#define DEBUG_PRINTF(...) printf("DEBUG: " __VA_ARGS__)
#else
#define DEBUG_PRINTF(...) do {} while (0)
#endif
// clang-format on
