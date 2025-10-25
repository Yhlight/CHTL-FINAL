import subprocess

def run_test():
    """Runs the test for the custom style parser."""
    try:
        # Build the project first to ensure the executable is up-to-date
        build_result = subprocess.run(['python3', 'build.py', 'build'], capture_output=True, text=True)
        if build_result.returncode != 0:
            print("Build failed!")
            print(build_result.stdout)
            print(build_result.stderr)
            return

        # Run the CHTL compiler on the custom_style.chtl file
        result = subprocess.run(
            ['./build/chtl', 'tests/custom_style.chtl'],
            capture_output=True,
            text=True,
            check=True
        )

        # Check if the output is correct
        expected_output = "ProgramNode({CustomStyleTemplateNode(TextSet, StyleNode({StylePropertyNode(color), StylePropertyNode(font-size), })), ElementNode(div, attributes={}, children={StyleNode({CustomStyleUsageNode(TextSet, StyleNode({StylePropertyNode(color: LiteralValueNode(red)), StylePropertyNode(font-size: LiteralValueNode(16px)), })), }), }), })\n"

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
