#!/bin/bash
./build/debug/src/markamp.app/Contents/MacOS/markamp &
PID=$!
sleep 2
sample $PID 1 10 > sample_output.txt
kill -9 $PID
