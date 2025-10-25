import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestMoreTemplateVarParser(unittest.TestCase):
    def test_more_template_var_parser(self):
        output = build_and_run("tests/more_template_var.chtl")
        expected_output = """
<style>
div {
  color: "black";
  font-size: 16px;
  padding: 10px;
  margin: 20px;
}
div {
  background-color: "rgb(255, 192, 203)";
}
</style>
<body><div></div><div></div></body>
"""
        self.maxDiff = None
        self.assertEqual(output.strip(), expected_output.strip())

    def test_more_template_var_parser_ast(self):
        output = build_and_run("tests/more_template_var.chtl", dump_ast=True)
        # We don't care about the order of the variables in the template, so we'll just check for the presence of the nodes
        self.assertIn("ProgramNode", output)
        self.assertIn("VarTemplateNode(ThemeColor", output)
        self.assertIn("VarTemplateNode(Spacing", output)
        self.assertIn("ElementTemplateNode(Box", output)
        self.assertIn("ElementTemplateUsageNode(Box)", output)

if __name__ == '__main__':
    unittest.main()
