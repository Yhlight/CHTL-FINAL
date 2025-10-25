import os
import subprocess

def run_command(command):
    """Runs a command and prints its output."""
    process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    for line in iter(process.stdout.readline, b''):
        print(line.decode('utf-8').strip())
    process.stdout.close()
    return process.wait()

def build():
    """Builds the CHTL project."""
    if not os.path.exists("build"):
        os.makedirs("build")
    os.chdir("build")
    run_command("cmake ..")
    run_command("cmake --build .")
    os.chdir("..")

def run(filepath):
    """Runs the CHTL compiler on a file."""
    if not os.path.exists("build/chtl"):
        print("CHTL compiler not found. Building...")
        build()

    if os.path.exists("build/chtl"):
        return run_command(f"build/chtl {filepath}")
    else:
        print("Failed to build CHTL compiler.")
        return 1

def build_and_run(filepath, dump_ast=False):
    """Builds and runs the CHTL compiler on a file, capturing the output."""
    build()
    # Check if the compiler exists after building
    if not os.path.exists("build/chtl"):
        print("Failed to build CHTL compiler.")
        return "Build failed"

    command = ["build/chtl", filepath]
    if dump_ast:
        command.append("--dump-ast")

    # Run the compiler and capture the output
    try:
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            check=True
        )
        return result.stdout
    except subprocess.CalledProcessError as e:
        return f"Execution failed: {e.stderr}"

if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1:
        if sys.argv[1] == "build":
            build()
        else:
            run(sys.argv[1])
    else:
        print("Usage: python build.py [build | <filepath>]")
