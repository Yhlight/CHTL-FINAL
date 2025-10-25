import subprocess

def run_test():
    """Runs the test for the 'Hello, World!' CHTL file."""
    try:
        # Run the CHTL compiler on the hello.chtl file
        result = subprocess.run(
            ['./build/chtl', 'tests/hello.chtl'],
            capture_output=True,
            text=True,
            check=True
        )

        # Check if the output is correct
        expected_output = "Hello, World!\n"
        if result.stdout == expected_output:
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
