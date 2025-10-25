import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestElementParser(unittest.TestCase):
    def test_element_parser(self):
        output = build_and_run("tests/element.chtl", dump_ast=True)
        expected_output = """ProgramNode({ElementNode(div, attributes={}, children={ElementNode(h1, attributes={}, children={TextNode("Hello, CHTL!"), }), ElementNode(p, attributes={}, children={TextNode("This is a test."), }), }), })"""
        self.assertEqual(output.strip(), expected_output.strip())

if __name__ == '__main__':
    unittest.main()
