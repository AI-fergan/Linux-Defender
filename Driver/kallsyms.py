import subprocess

command = "sudo cat /proc/kallsyms | grep kallsyms_lookup_name"

try:
    sys_call_table = 0
    # Run the shell command and capture the output
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Wait for the process to finish and get the output
    stdout, stderr = process.communicate()
    
    data = stdout.decode('utf-8')
    for line in data.split('\n'):
        line_data = line.split(' ')
        if 'kallsyms_lookup_name' == line_data[-1]:
            sys_call_table = int(line_data[0], 16)

    print(sys_call_table)


except Exception as e:
    print("Error:", str(e))

