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
        run_command(f"build/chtl {filepath}")
    else:
        print("Failed to build CHTL compiler.")

if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1:
        if sys.argv[1] == "build":
            build()
        else:
            run(sys.argv[1])
    else:
        print("Usage: python build.py [build | <filepath>]")
