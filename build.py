# build.py
import os
import subprocess
import sys
import platform

# CHTL项目根目录
CHTL_DIR = "CHTL"
BUILD_DIR = os.path.join(CHTL_DIR, "build")

def run_command(command, cwd):
    """在指定目录中运行命令，并检查错误。"""
    print(f"--- 在 {cwd} 中运行命令: {' '.join(command)} ---")
    try:
        # 在Windows上，某些命令（如cmake）可能需要shell=True
        use_shell = platform.system() == "Windows"
        result = subprocess.run(
            command,
            cwd=cwd,
            check=True,
            capture_output=True,
            text=True,
            encoding='utf-8',
            shell=use_shell
        )
        print(result.stdout)
        if result.stderr:
            print("--- Stderr ---")
            print(result.stderr)
        return True
    except FileNotFoundError as e:
        print(f"错误: 命令未找到: {e.filename}。 请确保它已安装并在您的PATH中。")
        return False
    except subprocess.CalledProcessError as e:
        print(f"错误: 命令 {' '.join(command)} 返回非零退出码 {e.returncode}。")
        print("--- Stdout ---")
        print(e.stdout)
        print("--- Stderr ---")
        print(e.stderr)
        return False
    except Exception as e:
        print(f"执行命令时发生未知错误: {e}")
        return False

def find_test_executables(directory):
    """在指定目录中递归地查找测试可执行文件。"""
    executables = []
    if not os.path.exists(directory) or not os.path.isdir(directory):
        return executables

    for root, dirs, files in os.walk(directory):
        for filename in files:
            if filename.endswith("Test"):
                filepath = os.path.join(root, filename)
                # 检查文件是否是可执行的
                if os.access(filepath, os.X_OK) and not os.path.isdir(filepath):
                    # 获取相对于build目录的路径
                    relative_path = os.path.relpath(filepath, directory)
                    executables.append(f"./{relative_path}")
    return executables

def main():
    """构建和测试CHTL项目的主函数。"""
    # 1. 确保构建目录存在
    if not os.path.exists(BUILD_DIR):
        print(f"创建构建目录: {BUILD_DIR}")
        os.makedirs(BUILD_DIR)

    # 2. 运行CMake
    if not run_command(["cmake", ".."], cwd=BUILD_DIR):
        sys.exit(1)

    # 3. 运行Make
    if not run_command(["make"], cwd=BUILD_DIR):
        sys.exit(1)

    # 4. 运行测试
    print("\n--- 构建完成。开始运行测试... ---")
    test_execs = find_test_executables(BUILD_DIR)

    if not test_execs:
        print("未找到测试可执行文件。跳过测试步骤。")
    else:
        print(f"找到测试: {', '.join(test_execs)}")
        for test in test_execs:
            if not run_command([test], cwd=BUILD_DIR):
                print(f"--- 测试失败: {test} ---")
                sys.exit(1)
            else:
                print(f"--- 测试通过: {test} ---")

    print("\n--- 构建和测试脚本成功完成。 ---")

if __name__ == "__main__":
    main()
