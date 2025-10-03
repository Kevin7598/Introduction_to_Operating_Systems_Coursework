# ECE4820J Lab 3
### Kaiqi Zhu 522370910091

## `diff` and `patch` commands

``` bash
# Edit a file
sudo nano /usr/src//linux-headers-5.15.0-156/Makefile

# Create patch using diff command
diff -ruN /usr/src_orig /usr/src > ~/mychanges.patch

# Apply the patch to copy
cd /usr
patch -p1 < ~/mychanges.patch

# Revert the patch
cd /usr
patch -R -p1 < ~/mychanges.patch
```

## Scripting and regular expressions

The command line: `ifconfig ens160 | awk '/inet / {print $2}'`

The output: 172.16.75.131