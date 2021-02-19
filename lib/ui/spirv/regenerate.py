import sys
import subprocess

if len(sys.argv) != 4:
    print 'Invalid number of arguments'
    sys.exit(1)

# Forward args [exectuable_name, input_name, output_name]
subprocess.call(sys.argv[1:])

