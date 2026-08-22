#!/bin/bash

PORT=${1:-4020}
SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$SCRIPT_DIR/../area" || { echo "Directory ../area not found!"; exit 1; }

PID=$(ss -ltnp "sport = :$PORT" | sed -n 's/.*pid=\([0-9]\+\).*/\1/p' | head -n 1)
if [ -z "$PID" ]; then
    echo "ZeroPoint is not running on port $PORT."
    exit 0
fi

# Let startup.sh recognise this as a deliberate shutdown after ZeroPoint exits.
touch shutdown.txt
kill -TERM "$PID"

for _ in $(seq 1 10); do
    if ! ss -ltn "sport = :$PORT" | grep -q LISTEN; then
        echo "ZeroPoint stopped."
        exit 0
    fi
    sleep 1
done

echo "ZeroPoint did not stop within 10 seconds."
exit 1