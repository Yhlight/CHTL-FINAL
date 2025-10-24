import os
import subprocess
import sys

def run_command(command):
    """Runs a command and prints its output."""
    try:
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        for line in iter(process.stdout.readline, ''):
            sys.stdout.write(line)
        process.stdout.close()
        return_code = process.wait()
        if return_code:
            raise subprocess.CalledProcessError(return_code, command)
    except FileNotFoundError:
        print(f"Error: Command '{command[0]}' not found. Make sure it's in your PATH.")
        sys.exit(1)

def main():
    """Main function to run the build process."""
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Configure
    print("--- Configuring with CMake ---")
    configure_command = ["cmake", "-S", ".", "-B", build_dir]
    run_command(configure_command)

    # Build
    print("\n--- Building with CMake ---")
    build_command = ["cmake", "--build", build_dir]
    run_command(build_command)

    # Run executable (optional)
    executable_name = "chtl"  # This should match the executable name in CMakeLists.txt
    executable_path = os.path.join(build_dir, executable_name)

    if os.path.exists(executable_path):
        print(f"\n--- Running {executable_name} ---")
        run_command([executable_path])
    else:
        print(f"\n--- Executable '{executable_name}' not found ---")

if __name__ == "__main__":
    main()
