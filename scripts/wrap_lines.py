#!/usr/bin/env python3
"""读取文件并将每行转换为 C++ 字符串字面量，末尾追加 "\\n"，便于粘贴到 C++ 代码中。

用法示例：
    python scripts/wrap_lines.py --input 01.Triangle/shader/vertex/vertex.glsl

输出示例（每行一条 C++ 字面量）：
    "line1\n"
    "line2\n"
"""

import argparse
import sys


def process_file(path: str, encoding: str = "utf-8", trim: bool = False):
    try:
        with open(path, "r", encoding=encoding, errors="surrogateescape") as f:
            for raw in f:
                # 去掉行尾的换行符（保留其它空白，除非 --trim 指定）
                line = raw.rstrip("\r\n")
                if trim:
                    line = line.strip()

                # 转义反斜和双引号，生成 C++ 字面量并在末尾追加 \n
                esc = line.replace("\\", "\\\\").replace('"', '\\"')
                yield f"\"{esc}\\n\""
    except FileNotFoundError:
        print(f"Error: file not found: {path}", file=sys.stderr)
        raise


def main():
    p = argparse.ArgumentParser(description="Convert each line of a file into a C++ string literal ending with \\n+    ")
    p.add_argument("--input", "-i", required=True, help="输入文件路径")
    p.add_argument("--encoding", default="utf-8", help="文件编码，默认 utf-8")
    p.add_argument("--trim", action="store_true", help="去除每行首尾空白")

    args = p.parse_args()

    try:
        for out in process_file(args.input, encoding=args.encoding, trim=args.trim):
            print(out)
    except Exception:
        sys.exit(2)


if __name__ == "__main__":
    main()
