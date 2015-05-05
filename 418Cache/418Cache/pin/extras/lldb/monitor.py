#!/usr/bin/python
import lldb
import commands
import re

def monitor_command(debugger, command, result, dict):
    """The monitor command implementation.
    Sends the command as expected by LLDB, and parses the response from PinADX
    as it is in GDB expected format.
    """
    result = lldb.SBCommandReturnObject()
    debugger.GetCommandInterpreter().HandleCommand("process plugin packet monitor " + command, result)
    result_str = result.GetOutput()
    stripped = re.search(".*response: ([0-9A-Fa-f]+)$", result_str).group(1)
    print stripped.decode("hex")

def __lldb_init_module (debugger, dict):
    monitor_command.__doc__ = "Sends arbitrary commands to the backend"

    debugger.HandleCommand('command script add -f monitor.monitor_command monitor')

    print 'The "monitor" command has been set.'
