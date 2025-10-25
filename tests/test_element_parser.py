import subprocess

def run_test():
    """Runs the test for the element parser."""
    try:
        # Build the project first to ensure the executable is up-to-date
        build_result = subprocess.run(['python3', 'build.py', 'build'], capture_output=True, text=True)
        if build_result.returncode != 0:
            print("Build failed!")
            print(build_result.stdout)
            print(build_result.stderr)
            return

        # Run the CHTL compiler on the element.chtl file
        result = subprocess.run(
            ['./build/chtl', 'tests/element.chtl'],
            capture_output=True,
            text=True,
            check=True
        )

        # Check if the output is correct
        expected_output = "ProgramNode({ElementNode(div, {ElementNode(h1, {TextNode(\"Hello, CHTL!\"), }), ElementNode(p, {TextNode(\"This is a test.\"), }), }), })\n"

        if result.stdout.strip() == expected_output.strip():
            print("Test passed!")
        else:
            print(f"Test failed: Expected '{expected_output}', but got '{result.stdout}'")

    except FileNotFoundError:
        print("Error: 'build/chtl' not found. Please build the project first.")
    except subprocess.CalledProcessError as e:
        print(f"Error running chtl compiler: {e}")
        print(f"Stderr: {e.stderr}")

if __name__ == "__main__":
    run_test()
