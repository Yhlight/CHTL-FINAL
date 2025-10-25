import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestImportParser(unittest.TestCase):
    def test_import_parser(self):
        output = build_and_run("tests/imports.chtl", dump_ast=True)
        expected_output = """ProgramNode({ImportNode("module1.chtl"), ImportNode("module2.chtl" as myModule), })"""
        self.assertEqual(output.strip(), expected_output.strip())

if __name__ == '__main__':
    unittest.main()
