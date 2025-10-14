#!/usr/bin/env python3
"""
CHTL项目构建脚本
支持跨平台构建、测试和运行CHTL编译器
"""

import os
import sys
import subprocess
import shutil
import argparse
from pathlib import Path


class CHTLBuilder:
    def __init__(self, build_type="Debug", build_dir="build", clean=False):
        self.project_root = Path(__file__).parent.resolve()
        self.build_type = build_type
        self.build_dir = self.project_root / build_dir
        self.clean = clean
        
    def clean_build(self):
        """清理构建目录"""
        if self.build_dir.exists():
            print(f"清理构建目录: {self.build_dir}")
            shutil.rmtree(self.build_dir)
    
    def configure(self):
        """配置CMake项目"""
        if self.clean:
            self.clean_build()
            
        self.build_dir.mkdir(parents=True, exist_ok=True)
        
        print(f"配置CMake项目 (构建类型: {self.build_type})...")
        cmd = [
            "cmake",
            "-S", str(self.project_root),
            "-B", str(self.build_dir),
            f"-DCMAKE_BUILD_TYPE={self.build_type}"
        ]
        
        result = subprocess.run(cmd, cwd=self.project_root)
        if result.returncode != 0:
            print("CMake配置失败！", file=sys.stderr)
            sys.exit(1)
        print("CMake配置成功！")
    
    def build(self):
        """构建项目"""
        print(f"构建CHTL项目...")
        cmd = [
            "cmake",
            "--build", str(self.build_dir),
            "--config", self.build_type,
            "-j", str(os.cpu_count() or 4)
        ]
        
        result = subprocess.run(cmd, cwd=self.project_root)
        if result.returncode != 0:
            print("构建失败！", file=sys.stderr)
            sys.exit(1)
        print("构建成功！")
    
    def test(self):
        """运行测试"""
        print("运行测试...")
        test_executable = self.build_dir / "bin" / "chtl_tests"
        
        if sys.platform == "win32":
            test_executable = test_executable.with_suffix(".exe")
        
        if not test_executable.exists():
            print(f"测试可执行文件不存在: {test_executable}", file=sys.stderr)
            print("请先运行构建命令", file=sys.stderr)
            sys.exit(1)
        
        result = subprocess.run([str(test_executable)], cwd=self.project_root)
        if result.returncode != 0:
            print("测试失败！", file=sys.stderr)
            sys.exit(1)
        print("所有测试通过！")
    
    def run(self, args=None):
        """运行CHTL编译器"""
        chtl_executable = self.build_dir / "bin" / "chtl"
        
        if sys.platform == "win32":
            chtl_executable = chtl_executable.with_suffix(".exe")
        
        if not chtl_executable.exists():
            print(f"CHTL可执行文件不存在: {chtl_executable}", file=sys.stderr)
            print("请先运行构建命令", file=sys.stderr)
            sys.exit(1)
        
        cmd = [str(chtl_executable)]
        if args:
            cmd.extend(args)
        
        print(f"运行CHTL编译器: {' '.join(cmd)}")
        result = subprocess.run(cmd, cwd=self.project_root)
        sys.exit(result.returncode)
    
    def find_chtl_binary(self):
        """查找已构建的CHTL二进制文件"""
        chtl_executable = self.build_dir / "bin" / "chtl"
        
        if sys.platform == "win32":
            chtl_executable = chtl_executable.with_suffix(".exe")
        
        if chtl_executable.exists():
            return chtl_executable
        return None


def main():
    parser = argparse.ArgumentParser(
        description="CHTL项目构建脚本",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  python build.py configure build test    # 配置、构建并测试
  python build.py build                   # 仅构建
  python build.py test                    # 仅运行测试
  python build.py run -- file.chtl        # 运行编译器
  python build.py clean build             # 清理后重新构建
        """
    )
    
    parser.add_argument(
        "commands",
        nargs="+",
        choices=["configure", "build", "test", "run", "clean", "all"],
        help="要执行的命令"
    )
    
    parser.add_argument(
        "--build-type",
        default="Debug",
        choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
        help="CMake构建类型 (默认: Debug)"
    )
    
    parser.add_argument(
        "--build-dir",
        default="build",
        help="构建目录 (默认: build)"
    )
    
    parser.add_argument(
        "--",
        dest="run_args",
        nargs=argparse.REMAINDER,
        help="传递给CHTL编译器的参数"
    )
    
    args = parser.parse_args()
    
    # 处理clean命令
    clean = "clean" in args.commands
    commands = [cmd for cmd in args.commands if cmd != "clean"]
    
    # 处理all命令
    if "all" in commands:
        commands = ["configure", "build", "test"]
    
    builder = CHTLBuilder(
        build_type=args.build_type,
        build_dir=args.build_dir,
        clean=clean
    )
    
    # 执行命令
    for cmd in commands:
        if cmd == "configure":
            builder.configure()
        elif cmd == "build":
            # 如果没有配置过，先配置
            if not (builder.build_dir / "CMakeCache.txt").exists():
                builder.configure()
            builder.build()
        elif cmd == "test":
            builder.test()
        elif cmd == "run":
            run_args = args.run_args if hasattr(args, 'run_args') else None
            builder.run(run_args)
    
    # 如果找到了二进制文件，显示路径
    binary = builder.find_chtl_binary()
    if binary and "build" in commands:
        print(f"\nCHTL编译器已构建: {binary}")


if __name__ == "__main__":
    main()
