# Create a new user.
sudo adduser kaiqi
# List all the currently running processes.
ps aux
# Display the characteristics of the CPU.
lscpu
# Display the characteristics of the available memory.
free -h
# Redirect random output read from a system device, into two different files.
dd if=/dev/urandom bs=1k count=1 2>/dev/null | tee file1 file2 >/dev/null
# Concatenate file 1 and file 2.
cat file1 file2 > combined_file
# Read the file as hexadecimal values.
hexdump -C combined_file
# Find specific files.
find . -type f -name "*mutex*" -exec grep -lw "nest_lock" {} \;