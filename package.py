import argparse
import subprocess
import sys
import os

def compile_to_ast(chtl_file):
    """Compiles a .chtl file to an AST and returns the serialized AST."""
    chtl_executable = os.path.join(os.path.dirname(__file__), "CHTL", "build", "chtl")
    if not os.path.exists(chtl_executable):
        raise FileNotFoundError(f"CHTML executable not found at {chtl_executable}")

    command = [chtl_executable, "serialize", chtl_file]
    result = subprocess.run(command, capture_output=True, check=True)
    return result.stdout

def main():
    """Main function to handle command-line arguments."""
    parser = argparse.ArgumentParser(description="CHTML Packaging Tool")
    parser.add_argument("file", help="The .chtl file to package.")
    parser.add_argument("-o", "--output", help="The output .cmod file path.")
    args = parser.parse_args()

    output_path = args.output
    if not output_path:
        output_path = os.path.splitext(args.file)[0] + ".cmod"

    try:
        serialized_ast = compile_to_ast(args.file)
        with open(output_path, "wb") as f:
            f.write(serialized_ast)
        print(f"Successfully packaged {args.file} to {output_path}")
    except FileNotFoundError as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    except subprocess.CalledProcessError as e:
        print(f"Error compiling {args.file}:", file=sys.stderr)
        print(e.stderr.decode(), file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"An unexpected error occurred: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
