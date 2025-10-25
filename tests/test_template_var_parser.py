import subprocess

def run_test():
    """Runs the test for the var template parser."""
    try:
        # Build the project first to ensure the executable is up-to-date
        build_result = subprocess.run(['python3', 'build.py', 'build'], capture_output=True, text=True)
        if build_result.returncode != 0:
            print("Build failed!")
            print(build_result.stdout)
            print(build_result.stderr)
            return

        # Run the CHTL compiler on the template_var.chtl file
        result = subprocess.run(
            ['./build/chtl', 'tests/template_var.chtl'],
            capture_output=True,
            text=True,
            check=True
        )

        # Check if the output is correct
        expected_output = "ProgramNode({VarTemplateNode(ThemeColor, {tableColor: LiteralValueNode(\"rgb(255, 192, 203)\"), }), ElementNode(div, attributes={}, children={StyleNode({StylePropertyNode(background-color: TemplateVarUsageNode(ThemeColor(tableColor))), }), }), })\n"

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
