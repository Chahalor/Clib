#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
BIN="/tmp/args_controls_test"

cd "$ROOT_DIR"

cmake --build build -j4 >/dev/null

cc -std=c11 -Wall -Wextra -Werror \
	-I. -I_internal_ -Istandards -Ifiles -Iinterface -Imemory -Inetwork -Irandom -Itimes \
	interface/args/controls/main.c build/libclib.a \
	-o "$BIN"

"$BIN"
