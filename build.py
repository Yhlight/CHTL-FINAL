#!/usr/bin/env python3
import argparse
import os
import shutil
import subprocess
import sys

def run_command(command, workdir='.'):
    """Runs a command and checks its return code."""
    print(f"Running command: {' '.join(command)} in {workdir}")
    process = subprocess.run(command, cwd=workdir)
    if process.returncode != 0:
        print(f"Error: Command '{' '.join(command)}' failed with exit code {process.returncode}")
        sys.exit(process.returncode)

def clean():
    """Removes the build directory."""
    print("Cleaning the build directory...")
    if os.path.exists("build"):
        shutil.rmtree("build")
    print("Clean complete.")

def build_project(build_type='Debug'):
    """Configures and builds the project using CMake."""
    print(f"Building project in {build_type} mode...")
    build_dir = "build"
    os.makedirs(build_dir, exist_ok=True)

    cmake_args = [
        "cmake",
        "-S", ".",
        "-B", build_dir,
        f"-DCMAKE_BUILD_TYPE={build_type}"
    ]
    run_command(cmake_args)

    build_args = ["cmake", "--build", build_dir]
    run_command(build_args)
    print("Build complete.")

def run_tests():
    """Runs the tests."""
    print("Running tests...")
    build_dir = "build"
    if not os.path.exists(build_dir):
        print("Build directory not found. Please build the project first.")
        sys.exit(1)

    ctest_args = ["ctest", "--output-on-failure"]
    run_command(ctest_args, workdir=build_dir)
    print("Tests complete.")

def main():
    parser = argparse.ArgumentParser(description="CHTL Project Build Script")
    subparsers = parser.add_subparsers(dest="command", required=True)

    subparsers.add_parser("clean", help="Clean the build directory.")

    build_parser = subparsers.add_parser("build", help="Build the project.")
    build_parser.add_argument(
        "--config",
        choices=['Debug', 'Release'],
        default='Debug',
        help="Build configuration (Debug or Release)."
    )

    subparsers.add_parser("test", help="Run tests.")

    args = parser.parse_args()

    # As per user instruction, always cd to /app
    os.chdir('/app')

    if args.command == "clean":
        clean()
    elif args.command == "build":
        build_project(args.config)
    elif args.command == "test":
        # A full build should happen before testing
        build_project()
        run_tests()

if __name__ == "__main__":
    main()