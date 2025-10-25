import os
import subprocess
import sys

def run_command(command):
    """Runs a command and prints its output."""
    try:
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        for line in process.stdout:
            print(line, end='')
        process.wait()
        if process.returncode != 0:
            print(f"Error: Command '{' '.join(command)}' failed with exit code {process.returncode}")
            sys.exit(process.returncode)
    except FileNotFoundError:
        print(f"Error: Command not found: {command[0]}")
        sys.exit(1)

def main():
    """Main function to build and run the CHTL compiler."""
    # Create build directory
    if not os.path.exists("build"):
        os.makedirs("build")

    # Change to build directory
    os.chdir("build")

    # Run CMake
    run_command(["cmake", ".."])

    # Build the project
    run_command(["cmake", "--build", "."])

    # Run tests
    print("\nRunning tests...")
    run_command(["ctest", "--output-on-failure"])

    # Run the executable
    executable_path = "./chtl"
    if os.path.exists(executable_path):
        print("\nRunning CHTL compiler...")
        run_command([executable_path])
    else:
        print(f"Error: Executable not found at {executable_path}")
        sys.exit(1)

if __name__ == "__main__":
    main()
