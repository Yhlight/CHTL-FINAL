import unittest
import os
import sys

# Add the parent directory to the Python path
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from build import build_and_run

class TestNamespaceParser(unittest.TestCase):
    def test_ast_output(self):
        # Path to the CHTL file
        chtl_file = "tests/namespace.chtl"

        # Run the build and get the AST output
        output = build_and_run(chtl_file, dump_ast=True)

        # Define the expected AST output
        expected_ast = """ProgramNode({NamespaceNode(space, {ElementTemplateNode(Box, {ElementNode(div, attributes={}, children={TextNode("Hello from namespace!"), }), }), }), ElementNode(body, attributes={}, children={ElementTemplateUsageNode(Box from space), }), })"""

        # Assert that the AST output matches the expected output
        self.assertEqual(output.strip(), expected_ast.strip())

if __name__ == '__main__':
    unittest.main()
