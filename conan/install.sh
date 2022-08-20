#!/bin/sh

CURRENT_DIR="$( dirname -- "$0" )"
conan create "$CURRENT_DIR" zhekehz/main
