#!/usr/bin/env python3
import os
import subprocess
import sys

def run_command(command, cwd=None):
    """Runs a command and prints its output."""
    try:
        process = subprocess.Popen(command, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        for line in iter(process.stdout.readline, ''):
            sys.stdout.write(line)
        process.stdout.close()
        return_code = process.wait()
        if return_code:
            raise subprocess.CalledProcessError(return_code, command)
    except FileNotFoundError:
        print(f"Error: Command '{command[0]}' not found. Make sure it's in your PATH.")
        sys.exit(1)
    except subprocess.CalledProcessError as e:
        print(f"Error: Command '{' '.join(e.cmd)}' returned non-zero exit status {e.returncode}.")
        sys.exit(1)

def main():
    """Main function to build and run the CHTL project."""
    build_dir = "build"

    # Create build directory if it doesn't exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Configure CMake
    print("--- Configuring CMake ---")
    run_command(["cmake", ".."], cwd=build_dir)

    # Build the project
    print("\n--- Building Project ---")
    run_command(["cmake", "--build", "."], cwd=build_dir)

    # Run the tests
    print("\n--- Running Tests ---")
    run_command(["ctest", "--output-on-failure"], cwd=build_dir)

    # Run the executable
    executable_path = os.path.join(build_dir, "chtl")
    if os.path.exists(executable_path):
        print("\n--- Running Executable ---")
        run_command([os.path.abspath(executable_path)])
    else:
        print(f"\nError: Executable not found at {executable_path}")
        sys.exit(1)

if __name__ == "__main__":
    main()
