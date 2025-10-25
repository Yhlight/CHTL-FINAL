import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from build import build_and_run

class TestGenerator(unittest.TestCase):
    def test_generator(self):
        output = build_and_run("tests/generator.chtl")
        expected_output = """
<style>
div {
  color: red;
  font-size: 16px;
}
</style>
<div class="container" id="main"><h1>Hello, World!</h1></div>
"""
        self.assertEqual(output.strip(), expected_output.strip())

if __name__ == '__main__':
    unittest.main()
