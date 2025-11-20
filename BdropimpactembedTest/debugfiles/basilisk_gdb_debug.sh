#!/bin/bash
# Save this as: basilisk_gdb_debug.sh
# Usage: ./basilisk_gdb_debug.sh ./Bdropimpactembed core

EXE=$1
CORE=$2

if [[ -z "$EXE" ]]; then
  echo "Usage: $0 <executable> [core-file]"
  exit 1
fi

echo "Launching GDB for $EXE ${CORE:+with core file $CORE}..."

# Start GDB with helpful aliases
gdb "$EXE" "$CORE" -ex "set pagination off" \
                   -ex "echo \n--- Stack trace (bt) ---\n" \
                   -ex "bt" \
                   -ex "echo \n--- Local variables ---\n" \
                   -ex "info locals" \
                   -ex "echo \n--- Function where crashed ---\n" \
                   -ex "frame" \
                   -ex "list" \
                   -ex "echo \nYou can now inspect variables (e.g. 'print f[]')\n"


