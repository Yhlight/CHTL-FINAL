import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestTemplateStyleParser(unittest.TestCase):
    def test_template_style_parser(self):
        output = build_and_run("tests/template_style.chtl")
        expected_output = """ProgramNode({StyleTemplateNode(DefaultText, StyleNode({StylePropertyNode(color: LiteralValueNode("black")), StylePropertyNode(line-height: LiteralValueNode(1.6)), })), ElementNode(div, attributes={}, children={StyleNode({StyleTemplateUsageNode(DefaultText), }), }), })"""
        self.assertEqual(output.strip(), expected_output.strip())

if __name__ == '__main__':
    unittest.main()
