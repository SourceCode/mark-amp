import lldb
import subprocess
import time

print("Starting MarkAmp...")
proc = subprocess.Popen(["./build/debug/src/markamp.app/Contents/MacOS/markamp"])
time.sleep(2) # Wait for hang

debugger = lldb.SBDebugger.Create()
debugger.SetAsync(False)
target = debugger.CreateTargetWithFileAndArch("./build/debug/src/markamp.app/Contents/MacOS/markamp", lldb.LLDB_ARCH_DEFAULT)
error = lldb.SBError()
process = target.AttachToProcessWithID(debugger.GetListener(), proc.pid, error)
if error.Fail():
    print(f"Failed to attach: {error}")
    proc.kill()
    exit()

print("Attached. Dumping thread stacks...")
for thread in process:
    print(f"\nThread {thread.GetThreadID()}:")
    for frame in thread:
        print(f"  {frame.GetFunctionName()}")

process.Kill()
