import os
import subprocess

def run_command(command, working_dir):
    """Runs a command in a specified working directory."""
    print(f"Executing: {' '.join(command)} in {working_dir}")
    process = subprocess.Popen(command, cwd=working_dir, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    for line in iter(process.stdout.readline, ''):
        print(line, end='')
    process.wait()
    if process.returncode != 0:
        raise RuntimeError(f"Command failed with exit code {process.returncode}: {' '.join(command)}")

def main():
    """Main function to orchestrate the build process."""
    project_dir = os.path.dirname(os.path.abspath(__file__))
    build_dir = os.path.join(project_dir, "build")

    # Create build directory if it doesn't exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    try:
        # Configure the project with CMake
        run_command(["cmake", ".."], build_dir)

        # Build the project with Make
        run_command(["make"], build_dir)

        # Run the tests with CTest
        run_command(["ctest", "--verbose"], build_dir)

        print("\nBuild and test process completed successfully.")

    except (RuntimeError, FileNotFoundError) as e:
        print(f"\nAn error occurred: {e}")

if __name__ == "__main__":
    main()