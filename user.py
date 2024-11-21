import os

# Path to the sysfs file for the sysfs attribute
sysfs_file_path = '/sys/class/Sysfs_class/sysfs_class_sysfs_Device/sysfs_att'

def read_sysfs():
    try:
        with open(sysfs_file_path, 'r') as f:
            value = f.read().strip()
            print("Current value of sysfs_int: {}".format(value))  # Updated for Python 3.5 compatibility
    except FileNotFoundError:
        print("Error: {} not found. Make sure the module is loaded.".format(sysfs_file_path))
    except Exception as e:
        print("Error reading from sysfs: {}".format(e))

def write_sysfs(new_value):
    try:
        with open(sysfs_file_path, 'w') as f:
            f.write("{}\n".format(new_value))  # Updated for Python 3.5 compatibility
        print("Successfully wrote new value: {}".format(new_value))  # Updated for Python 3.5 compatibility
    except FileNotFoundError:
        print("Error: {} not found. Make sure the module is loaded.".format(sysfs_file_path))
    except Exception as e:
        print("Error writing to sysfs: {}".format(e))

def main():
    while True:
        # Display menu
        print("\nChoose an option:")
        print("1. Read sysfs_int value")
        print("2. Write new value to sysfs_int")
        print("3. Exit")
        
        choice = input("Enter your choice (1/2/3): ").strip()

        if choice == '1':
            # Read the current value of sysfs_int
            read_sysfs()
        elif choice == '2':
            # Write a new value to sysfs_int
            new_value = input("Enter new value for sysfs_int: ").strip()
            if new_value.isdigit():  # Ensure the input is a valid integer
                write_sysfs(new_value)
            else:
                print("Error: Please enter a valid integer.")
        elif choice == '3':
            print("Exiting...")
            break
        else:
            print("Invalid choice, please try again.")

if __name__ == '__main__':
    main()
