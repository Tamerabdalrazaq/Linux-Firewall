import os
import sys

# Path to the sysfs file for the sysfs attribute
sysfs_file_path = '/sys/class/Sysfs_class/sysfs_class_sysfs_Device/sysfs_att'

def read_sysfs():
    try:
        with open(sysfs_file_path, 'r') as f:
            value = f.read().strip()
            print(value)  
    except FileNotFoundError:
        print("Error: {} not found. Make sure the module is loaded.".format(sysfs_file_path))
    except Exception as e:
        print("Error reading from sysfs: {}".format(e))

def write_sysfs():
    try:
        with open(sysfs_file_path, 'w') as f:
            f.write("{}\n".format(0))
    except FileNotFoundError:
        print("Error: {} not found. Make sure the module is loaded.".format(sysfs_file_path))
    except Exception as e:
        print("Error writing to sysfs: {}".format(e))

def main():
        args = sys.argv
        # Check correct number of arguments
        if len(args) > 2:
            return sys.exit("Error: Invalid Input")
        # Reset stats
        elif len(args) == 2:
            param = args[1]
            if param != "1":
                return sys.exit("Error: Invalid Input")
            write_sysfs()
        # Read Stats
        else:
            read_sysfs()

if __name__ == '__main__':
    main()
