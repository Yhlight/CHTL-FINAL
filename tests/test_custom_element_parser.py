import subprocess
import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestCustomElementParser(unittest.TestCase):
    def test_custom_element_parser(self):
        output = build_and_run("tests/custom_element.chtl")
        expected_output = """ProgramNode({CustomElementNode(MyComponent, {ElementNode(div, attributes={}, children={ElementNode(p, attributes={}, children={TextNode("Hello"), }), }), }), ElementNode(MyComponent, attributes={}, children={}), })"""
        self.assertEqual(output.strip(), expected_output.strip())

if __name__ == '__main__':
    unittest.main()
