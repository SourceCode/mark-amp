import lldb
import time
import os
import subprocess

# Launch the process
print("Launching MarkAmp...")
exe = "./build/debug/src/markamp.app/Contents/MacOS/markamp"
proc = subprocess.Popen([exe])
time.sleep(2) # Wait for it to hang

print("Attaching LLDB to PID", proc.pid)

debugger = lldb.SBDebugger.Create()
debugger.SetAsync(False)
target = debugger.CreateTargetWithFileAndArch(exe, lldb.LLDB_ARCH_DEFAULT)

error = lldb.SBError()
process = target.AttachToProcessWithID(debugger.GetListener(), proc.pid, error)

if error.Fail():
    print("Failed to attach:", error)
    proc.kill()
    exit(1)

print("Attached successfully. Dumping threads:")
for thread in process:
    print(f"\nThread {thread.GetThreadID()}:")
    for frame in thread:
        print(f"  {frame.GetFunctionName()}")

process.Kill()
print("Done.")
