#!/bin/bash
./build/debug/src/markamp.app/Contents/MacOS/markamp > /dev/null 2>&1 &
PID=$!
sleep 3
lldb -p $PID -o "bt all" -o "quit" > lldb_out.txt 2>&1
kill -9 $PID
