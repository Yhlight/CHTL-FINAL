import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestImportParser(unittest.TestCase):
    def test_import_parser(self):
        output = build_and_run("tests/imports.chtl", dump_ast=True)
        expected_output = """
ProgramNode({ImportNode("@Html", path="path/to/file.html", alias=myHtml), ImportNode("[Custom] @Element MyElement", path="path/to/file.chtl", alias=), ImportNode("[Template] @Style MyStyle", path="path/to/another.chtl", alias=myStyle), })
"""
        self.assertEqual("".join(output.split()), "".join(expected_output.split()))

if __name__ == '__main__':
    unittest.main()
