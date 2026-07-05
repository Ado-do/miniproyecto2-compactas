#!/bin/bash
set -e

./compile.sh
ctest --test-dir build -V
