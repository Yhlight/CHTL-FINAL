#!/usr/bin/env python3
"""
CHTL 项目构建脚本

功能：
1. 配置和构建 CMake 项目
2. 运行测试
3. 查找和调用生成的二进制文件
"""

import os
import sys
import subprocess
import argparse
import platform
from pathlib import Path


class CHTLBuilder:
    def __init__(self, project_root=None):
        self.project_root = Path(project_root or os.getcwd())
        self.build_dir = self.project_root / "build"
        self.bin_dir = self.build_dir / "bin"
        
        # 检测操作系统
        self.is_windows = platform.system() == "Windows"
        self.exe_suffix = ".exe" if self.is_windows else ""
        
    def clean(self):
        """清理构建目录"""
        print("🧹 清理构建目录...")
        if self.build_dir.exists():
            import shutil
            shutil.rmtree(self.build_dir)
            print("✅ 构建目录已清理")
        else:
            print("ℹ️  构建目录不存在，跳过清理")
    
    def configure(self, build_type="Debug"):
        """配置 CMake 项目"""
        print(f"⚙️  配置 CMake 项目 (构建类型: {build_type})...")
        
        self.build_dir.mkdir(exist_ok=True)
        
        cmd = [
            "cmake",
            "-S", str(self.project_root),
            "-B", str(self.build_dir),
            f"-DCMAKE_BUILD_TYPE={build_type}"
        ]
        
        try:
            subprocess.run(cmd, check=True)
            print("✅ CMake 配置完成")
            return True
        except subprocess.CalledProcessError as e:
            print(f"❌ CMake 配置失败: {e}")
            return False
        except FileNotFoundError:
            print("❌ 错误: 未找到 cmake 命令。请确保 CMake 已安装并在 PATH 中。")
            return False
    
    def build(self, target=None):
        """构建项目"""
        print("🔨 构建项目...")
        
        cmd = ["cmake", "--build", str(self.build_dir)]
        if target:
            cmd.extend(["--target", target])
        
        # 使用多核编译
        if not self.is_windows:
            import multiprocessing
            jobs = multiprocessing.cpu_count()
            cmd.extend(["--", f"-j{jobs}"])
        
        try:
            subprocess.run(cmd, check=True)
            print("✅ 构建完成")
            return True
        except subprocess.CalledProcessError as e:
            print(f"❌ 构建失败: {e}")
            return False
    
    def test(self):
        """运行测试"""
        print("🧪 运行测试...")
        
        test_exe = self.bin_dir / f"chtl_tests{self.exe_suffix}"
        
        if not test_exe.exists():
            print(f"❌ 测试可执行文件不存在: {test_exe}")
            return False
        
        try:
            subprocess.run([str(test_exe)], check=True)
            print("✅ 所有测试通过")
            return True
        except subprocess.CalledProcessError as e:
            print(f"❌ 测试失败: {e}")
            return False
    
    def run(self, input_file, args=None):
        """运行 CHTL 编译器"""
        print(f"🚀 运行 CHTL 编译器...")
        
        chtl_exe = self.bin_dir / f"chtl{self.exe_suffix}"
        
        if not chtl_exe.exists():
            print(f"❌ CHTL 可执行文件不存在: {chtl_exe}")
            print("💡 提示: 请先运行构建命令")
            return False
        
        cmd = [str(chtl_exe), input_file]
        if args:
            cmd.extend(args)
        
        try:
            subprocess.run(cmd, check=True)
            return True
        except subprocess.CalledProcessError as e:
            print(f"❌ 运行失败: {e}")
            return False
    
    def find_binary(self, name):
        """查找生成的二进制文件"""
        binary_path = self.bin_dir / f"{name}{self.exe_suffix}"
        
        if binary_path.exists():
            return binary_path
        else:
            print(f"❌ 未找到二进制文件: {name}")
            return None
    
    def list_binaries(self):
        """列出所有生成的二进制文件"""
        print("📦 已生成的二进制文件:")
        
        if not self.bin_dir.exists():
            print("  (构建目录不存在)")
            return
        
        binaries = list(self.bin_dir.glob(f"*{self.exe_suffix}"))
        
        if not binaries:
            print("  (未找到任何二进制文件)")
        else:
            for binary in binaries:
                size = binary.stat().st_size / 1024  # KB
                print(f"  • {binary.name} ({size:.1f} KB)")


def main():
    parser = argparse.ArgumentParser(
        description="CHTL 项目构建脚本",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  python build.py --configure --build    # 配置并构建项目
  python build.py --test                 # 运行测试
  python build.py --run example.chtl     # 运行编译器
  python build.py --clean --configure --build --test  # 完整构建流程
        """
    )
    
    parser.add_argument("--clean", action="store_true", help="清理构建目录")
    parser.add_argument("--configure", action="store_true", help="配置 CMake 项目")
    parser.add_argument("--build", action="store_true", help="构建项目")
    parser.add_argument("--test", action="store_true", help="运行测试")
    parser.add_argument("--run", metavar="FILE", help="运行 CHTL 编译器")
    parser.add_argument("--compile", metavar="FILE", help="编译 CHTL 文件并输出 HTML")
    parser.add_argument("--output", metavar="FILE", help="指定输出文件（配合 --compile 使用）")
    parser.add_argument("--list", action="store_true", help="列出所有生成的二进制文件")
    parser.add_argument("--build-type", default="Debug", 
                       choices=["Debug", "Release"], 
                       help="构建类型 (默认: Debug)")
    parser.add_argument("--all", action="store_true", 
                       help="执行完整构建流程 (clean + configure + build + test)")
    
    args = parser.parse_args()
    
    builder = CHTLBuilder()
    
    # 如果没有指定任何操作，显示帮助
    if not any([args.clean, args.configure, args.build, args.test, 
                args.run, args.list, args.all]):
        parser.print_help()
        return 0
    
    success = True
    
    # 执行完整流程
    if args.all:
        builder.clean()
        if not builder.configure(args.build_type):
            return 1
        if not builder.build():
            return 1
        if not builder.test():
            return 1
        builder.list_binaries()
        return 0
    
    # 执行指定的操作
    if args.clean:
        builder.clean()
    
    if args.configure:
        if not builder.configure(args.build_type):
            success = False
    
    if args.build and success:
        if not builder.build():
            success = False
    
    if args.test and success:
        if not builder.test():
            success = False
    
    if args.run and success:
        if not builder.run(args.run):
            success = False
    
    if args.compile and success:
        extra_args = []
        if args.output:
            extra_args.extend(["--output", args.output])
        if not builder.run(args.compile, extra_args):
            success = False
    
    if args.list:
        builder.list_binaries()
    
    return 0 if success else 1


if __name__ == "__main__":
    sys.exit(main())
