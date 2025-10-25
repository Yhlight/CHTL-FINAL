import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestAttributeParser(unittest.TestCase):
    def test_attribute_parser(self):
        output = build_and_run("tests/attributes.chtl", dump_ast=True)
        expected_output = """ProgramNode({ElementNode(div, attributes={class: LiteralValueNode("container"), id: LiteralValueNode("main"), }, children={ElementNode(h1, attributes={}, children={TextNode("Welcome!"), }), }), })"""
        self.assertEqual(output.strip(), expected_output.strip())

if __name__ == '__main__':
    unittest.main()
