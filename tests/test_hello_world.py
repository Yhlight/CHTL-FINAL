import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestHelloWorld(unittest.TestCase):
    def test_hello_world(self):
        output = build_and_run("tests/hello.chtl", dump_ast=True)
        expected_output = """ProgramNode({TextNode("Hello, World!"), })"""
        self.assertEqual(output.strip(), expected_output.strip())

if __name__ == '__main__':
    unittest.main()
