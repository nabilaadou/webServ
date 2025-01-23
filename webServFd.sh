#!/bin/bash

# Find the PID of the `webServ` process
PID=$(ps aux | grep './webServ' | grep -v grep | awk '{print $2}' | head -n 1)

# Check if PID is empty (process not running)
if [ -z "$PID" ]; then
    echo "webServ process not found."
    exit 1
fi

# Display the PID
echo "webServ process found with PID: $PID."
# List open file descriptors for the process
ls -l /proc/$PID/fd
