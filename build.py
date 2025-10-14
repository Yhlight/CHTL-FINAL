#!/usr/bin/env python3
"""
CHTL项目CMake构建脚本
支持构建、测试、清理和运行生成的二进制文件
"""

import os
import sys
import subprocess
import argparse
import shutil
from pathlib import Path

class CHTLBuilder:
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.build_dir = self.project_root / "build"
        self.bin_dir = self.build_dir / "bin"
        
    def run_command(self, command, cwd=None):
        """运行命令并返回结果"""
        try:
            result = subprocess.run(
                command, 
                shell=True, 
                cwd=cwd or self.project_root,
                capture_output=True, 
                text=True,
                check=True
            )
            return result.returncode, result.stdout, result.stderr
        except subprocess.CalledProcessError as e:
            return e.returncode, e.stdout, e.stderr
    
    def check_cmake(self):
        """检查CMake是否可用"""
        returncode, stdout, stderr = self.run_command("cmake --version")
        if returncode != 0:
            print("错误: 未找到CMake，请先安装CMake")
            return False
        print(f"CMake版本: {stdout.split()[2]}")
        return True
    
    def check_compiler(self):
        """检查C++编译器是否可用"""
        # 检查g++或clang++
        for compiler in ["g++", "clang++"]:
            returncode, stdout, stderr = self.run_command(f"{compiler} --version")
            if returncode == 0:
                print(f"找到C++编译器: {compiler}")
                return True
        print("错误: 未找到C++编译器，请安装g++或clang++")
        return False
    
    def configure(self, generator=None, build_type="Release"):
        """配置CMake项目"""
        if not self.check_cmake() or not self.check_compiler():
            return False
            
        # 创建构建目录
        self.build_dir.mkdir(exist_ok=True)
        
        # 构建CMake命令
        cmake_cmd = ["cmake", "-S", str(self.project_root), "-B", str(self.build_dir)]
        
        if generator:
            cmake_cmd.extend(["-G", generator])
        
        cmake_cmd.extend(["-DCMAKE_BUILD_TYPE=" + build_type])
        
        print(f"配置CMake项目...")
        print(f"命令: {' '.join(cmake_cmd)}")
        
        returncode, stdout, stderr = self.run_command(" ".join(cmake_cmd))
        if returncode != 0:
            print(f"CMake配置失败:")
            print(f"stdout: {stdout}")
            print(f"stderr: {stderr}")
            return False
        
        print("CMake配置成功!")
        return True
    
    def build(self, target=None, parallel=True):
        """构建项目"""
        if not self.build_dir.exists():
            print("错误: 构建目录不存在，请先运行配置")
            return False
        
        # 构建CMake命令
        cmake_cmd = ["cmake", "--build", str(self.build_dir)]
        
        if target:
            cmake_cmd.extend(["--target", target])
        
        if parallel:
            cmake_cmd.append("--parallel")
        
        print(f"构建项目...")
        print(f"命令: {' '.join(cmake_cmd)}")
        
        returncode, stdout, stderr = self.run_command(" ".join(cmake_cmd))
        if returncode != 0:
            print(f"构建失败:")
            print(f"stdout: {stdout}")
            print(f"stderr: {stderr}")
            return False
        
        print("构建成功!")
        return True
    
    def test(self):
        """运行测试"""
        if not self.build_dir.exists():
            print("错误: 构建目录不存在，请先运行构建")
            return False
        
        print("运行测试...")
        returncode, stdout, stderr = self.run_command("ctest --test-dir " + str(self.build_dir))
        
        if returncode != 0:
            print(f"测试失败:")
            print(f"stdout: {stdout}")
            print(f"stderr: {stderr}")
            return False
        
        print("测试通过!")
        return True
    
    def clean(self):
        """清理构建产物"""
        if self.build_dir.exists():
            print("清理构建目录...")
            shutil.rmtree(self.build_dir)
            print("清理完成!")
        else:
            print("构建目录不存在，无需清理")
    
    def run_compiler(self, input_file, output_dir=None, args=None):
        """运行CHTL编译器"""
        compiler_path = self.bin_dir / "chtl_compiler"
        if not compiler_path.exists():
            print(f"错误: 编译器不存在: {compiler_path}")
            print("请先运行构建命令")
            return False
        
        cmd = [str(compiler_path), str(input_file)]
        if output_dir:
            cmd.extend(["-o", str(output_dir)])
        if args:
            cmd.extend(args)
        
        print(f"运行CHTL编译器...")
        print(f"命令: {' '.join(cmd)}")
        
        returncode, stdout, stderr = self.run_command(" ".join(cmd))
        if returncode != 0:
            print(f"编译失败:")
            print(f"stdout: {stdout}")
            print(f"stderr: {stderr}")
            return False
        
        print("编译成功!")
        return True
    
    def run_js_compiler(self, input_file, output_dir=None, args=None):
        """运行CHTL JS编译器"""
        compiler_path = self.bin_dir / "chtl_js_compiler"
        if not compiler_path.exists():
            print(f"错误: CHTL JS编译器不存在: {compiler_path}")
            print("请先运行构建命令")
            return False
        
        cmd = [str(compiler_path), str(input_file)]
        if output_dir:
            cmd.extend(["-o", str(output_dir)])
        if args:
            cmd.extend(args)
        
        print(f"运行CHTL JS编译器...")
        print(f"命令: {' '.join(cmd)}")
        
        returncode, stdout, stderr = self.run_command(" ".join(cmd))
        if returncode != 0:
            print(f"编译失败:")
            print(f"stdout: {stdout}")
            print(f"stderr: {stderr}")
            return False
        
        print("编译成功!")
        return True
    
    def list_binaries(self):
        """列出生成的二进制文件"""
        if not self.bin_dir.exists():
            print("二进制目录不存在，请先运行构建")
            return
        
        print("生成的二进制文件:")
        for binary in self.bin_dir.iterdir():
            if binary.is_file() and binary.stat().st_size > 0:
                size = binary.stat().st_size
                print(f"  {binary.name} ({size} bytes)")

def main():
    parser = argparse.ArgumentParser(description="CHTL项目构建脚本")
    parser.add_argument("command", choices=[
        "configure", "build", "test", "clean", "run", "run-js", "list"
    ], help="要执行的命令")
    parser.add_argument("-g", "--generator", help="CMake生成器")
    parser.add_argument("-t", "--build-type", default="Release", 
                       choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
                       help="构建类型")
    parser.add_argument("-T", "--target", help="构建目标")
    parser.add_argument("-i", "--input", help="输入文件")
    parser.add_argument("-o", "--output", help="输出目录")
    parser.add_argument("--args", nargs="*", help="额外的命令行参数")
    parser.add_argument("--no-parallel", action="store_true", help="禁用并行构建")
    
    args = parser.parse_args()
    
    builder = CHTLBuilder()
    
    if args.command == "configure":
        success = builder.configure(args.generator, args.build_type)
        sys.exit(0 if success else 1)
    
    elif args.command == "build":
        success = builder.build(args.target, not args.no_parallel)
        sys.exit(0 if success else 1)
    
    elif args.command == "test":
        success = builder.test()
        sys.exit(0 if success else 1)
    
    elif args.command == "clean":
        builder.clean()
        sys.exit(0)
    
    elif args.command == "run":
        if not args.input:
            print("错误: 运行编译器需要指定输入文件 (-i)")
            sys.exit(1)
        success = builder.run_compiler(args.input, args.output, args.args)
        sys.exit(0 if success else 1)
    
    elif args.command == "run-js":
        if not args.input:
            print("错误: 运行CHTL JS编译器需要指定输入文件 (-i)")
            sys.exit(1)
        success = builder.run_js_compiler(args.input, args.output, args.args)
        sys.exit(0 if success else 1)
    
    elif args.command == "list":
        builder.list_binaries()
        sys.exit(0)

if __name__ == "__main__":
    main()