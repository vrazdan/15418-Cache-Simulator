import os
import sys

if len(sys.argv) != 3:
	print "Usage: run_pinatrace <inputprogram> <outputfile>\n Program must exist within 418Cache/programs. File will be output to ../traces"
	sys.exit();

os.system("../pin/pin -t ../pin/source/tools/ManualExamples/obj-intel64/pinatrace.dylib -- ../programs/" + sys.argv[1])
os.system("mv user.trace ../traces/"+sys.argv[2]+".trace")