import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestCustomStyleParser(unittest.TestCase):
    def test_custom_style_parser(self):
        output = build_and_run("tests/custom_style.chtl")
        expected_output = """
<style>
div {
  color: red;
  font-size: 16px;
}
</style>
<div></div>
"""
        self.assertEqual(output.strip(), expected_output.strip())

if __name__ == '__main__':
    unittest.main()
