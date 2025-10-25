import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestTemplateVarParser(unittest.TestCase):
    def test_template_var_parser(self):
        output = build_and_run("tests/template_var.chtl")
        expected_output = """ProgramNode({VarTemplateNode(ThemeColor, {tableColor: LiteralValueNode("rgb(255, 192, 203)"), }), ElementNode(div, attributes={}, children={StyleNode({StylePropertyNode(background-color: TemplateVarUsageNode(ThemeColor(tableColor))), }), }), })"""
        self.assertEqual(output.strip(), expected_output.strip())

if __name__ == '__main__':
    unittest.main()
