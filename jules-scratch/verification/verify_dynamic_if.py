
import os
import subprocess
from playwright.sync_api import sync_playwright

def run_chtl_compiler(input_file):
    """Runs the CHTL compiler to generate HTML."""
    compiler_path = "./CHTL/build/chtl"
    if not os.path.exists(compiler_path):
        raise FileNotFoundError("CHTL compiler not found. Please build the project first.")

    result = subprocess.run([compiler_path, "compile", input_file], capture_output=True, text=True)
    if result.returncode != 0:
        print("Compiler stderr:", result.stderr)
        raise RuntimeError("CHTL compiler failed to execute.")
    return result.stdout

def verify_frontend():
    """Generates HTML, opens it in Playwright, and takes a screenshot."""
    # 1. Arrange: Create CHTL and HTML file paths
    verification_dir = "jules-scratch/verification"
    chtl_input_file = os.path.join(verification_dir, "test.chtl")
    screenshot_path = os.path.join(verification_dir, "verification.png")

    # 2. Define CHTL content with a dynamic if block
    chtl_content = """
div {
    if {
        condition: $show$;
        span { text: "This text is visible"; }
    }
    script {
        let show = true;
    }
}
"""
    with open(chtl_input_file, "w") as f:
        f.write(chtl_content)

    # 3. Act: Run the CHTL compiler
    html_content = run_chtl_compiler(chtl_input_file)
    print("Successfully compiled CHTL to HTML string")

    # 4. Use Playwright to verify
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True)
        page = browser.new_page()

        page.set_content(html_content)

        # Give the script a moment to run (though it should be very fast)
        page.wait_for_timeout(100)

        # Take screenshot
        page.screenshot(path=screenshot_path)
        print(f"Screenshot saved to {screenshot_path}")

        browser.close()

if __name__ == "__main__":
    verify_frontend()
