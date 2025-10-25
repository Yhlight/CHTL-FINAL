import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestStyleParser(unittest.TestCase):
    def test_style_parser(self):
        output = build_and_run("tests/style.chtl")
        expected_output = """ProgramNode({ElementNode(div, attributes={}, children={StyleNode({StylePropertyNode(color: LiteralValueNode(red)), StylePropertyNode(font-size: LiteralValueNode(16px)), }), }), })"""
        self.assertEqual(output.strip(), expected_output.strip())

if __name__ == '__main__':
    unittest.main()
