import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestTemplateElementParser(unittest.TestCase):
    def test_template_element_parser(self):
        output = build_and_run("tests/template_element.chtl")
        expected_output = """ProgramNode({ElementTemplateNode(Box, {ElementNode(div, attributes={}, children={TextNode("This is a box."), }), }), ElementNode(body, attributes={}, children={ElementTemplateUsageNode(Box), }), })"""
        self.assertEqual(output.strip(), expected_output.strip())

if __name__ == '__main__':
    unittest.main()
