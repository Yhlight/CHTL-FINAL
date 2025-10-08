import os
import subprocess
import sys

def main():
    """
    Builds and tests the CHTL project using CMake.
    """
    project_root = os.path.dirname(os.path.abspath(__file__))
    build_dir = os.path.join(project_root, "build")

    # 1. Create build directory if it doesn't exist
    if not os.path.exists(build_dir):
        print(f"Creating build directory: {build_dir}")
        os.makedirs(build_dir)

    # 2. Configure with CMake
    cmake_args = ["cmake", project_root]
    print(f"Running command: {' '.join(cmake_args)} in {build_dir}")
    result = subprocess.run(cmake_args, cwd=build_dir, check=False)
    if result.returncode != 0:
        print("CMake configuration failed.", file=sys.stderr)
        sys.exit(result.returncode)

    # 3. Build with CMake
    build_args = ["cmake", "--build", "."]
    print(f"Running command: {' '.join(build_args)} in {build_dir}")
    result = subprocess.run(build_args, cwd=build_dir, check=False)
    if result.returncode != 0:
        print("Build failed.", file=sys.stderr)
        sys.exit(result.returncode)

    print("\nBuild successful!")

    # 4. Run tests with CTest
    print("\nRunning tests...")
    test_args = ["ctest", "--output-on-failure"]
    print(f"Running command: {' '.join(test_args)} in {build_dir}")
    result = subprocess.run(test_args, cwd=build_dir, check=False)
    if result.returncode != 0:
        print("Tests failed.", file=sys.stderr)
        sys.exit(result.returncode)

    print("\nAll tests passed!")

if __name__ == "__main__":
    main()