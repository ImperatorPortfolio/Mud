#!/bin/bash

# Set the port number
PORT=${1:-4020}
STREAM_LOG=${ZEROPOINT_STREAM_LOG:-0}

# Change to area directory.
cd ../area || { echo "Directory ../area not found!"; exit 1; }

# Set limits
ulimit -c unlimited
rm -f shutdown.txt

while true; do
    # Find the next available log index
    INDEX=1000
    while [ -e "../log/$INDEX.log" ]; do
        ((INDEX++))
    done
    LOGFILE="../log/$INDEX.log"

    # Record starting time
    date > "$LOGFILE"
    date > boot.txt

    # Check if port is already in use using ss (more modern than netstat)
    if ss -tuln | grep -q ":$PORT "; then
        echo "Port $PORT is already in use."
        exit 1
    fi

    # Interactive launchers can mirror the active log without changing the
    # normal detached-server behavior.
    if [ "$STREAM_LOG" = "1" ]; then
        echo "Monitoring $LOGFILE"
        ../src/ZeroPoint "$PORT" 2>&1 | tee -a "$LOGFILE"
    else
        ../src/ZeroPoint "$PORT" >> "$LOGFILE" 2>&1
    fi

    # Check for clean shutdown
    if [ -f "shutdown.txt" ]; then
        rm -f shutdown.txt
        exit 0
    fi

    # Wait before restarting
    sleep 5
done
