#!/bin/bash
#
#
#

MODULES_FILE='.modules'

set -e

if ! command -v modulectl >/dev/null 2>&1; then
	echo "modulectl is not installed"
	exit 1
fi

if [ ! -f "$MODULES_FILE" ]; then
	echo "missing $MODULES_FILE file"
	exit 1
fi

checks=(
	files/json
	interface/args
	interface/log
	memory/
	network/communication
	network/protocols
	times
	random
)

for check in "${checks[@]}"; do
	modulectl check "$check"
done