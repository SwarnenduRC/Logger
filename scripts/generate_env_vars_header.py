#!/usr/bin/env python3
import os
import re

# Updated path to include directory
HEADER_DIR = "include"
HEADER_FILE = os.path.join(HEADER_DIR, "ENV_VARS.hpp")

MIT_LICENSE = """\
/*
 * MIT License
 * 
 * Copyright (c) 2025 Swarnendu RC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * Author: Swarnendu RC
 *
 * NOTE: This file is auto-generated. Do NOT modify this file manually.
 *       If changes are needed, please modify the generating Python script:
 *       generate_env_vars_header.py
 */
"""

def quote_string(value):
    return f'"{value}"'

def file_size_to_expression(size_str):
    size_str = size_str.strip()
    match = re.fullmatch(r'(\d+)([KkMmGg]?[Bb]?)?', size_str)
    if not match:
        raise ValueError(f"Invalid FILE_SIZE format: '{size_str}'")

    num = int(match.group(1))
    unit = match.group(2).lower() if match.group(2) else ''

    if unit in ['', 'b']:
        return str(num)
    elif unit in ['k', 'kb']:
        return f"({num} * 1024)"
    elif unit in ['m', 'mb']:
        return f"({num} * 1024 * 1024)"
    elif unit in ['g', 'gb']:
        return f"({num} * 1024 * 1024 * 1024)"
    else:
        raise ValueError(f"Unknown unit in FILE_SIZE: '{unit}'")

def main():
    file_logging = os.getenv('FILE_LOGGING', '').lower()
    file_size = os.getenv('FILE_SIZE', '')
    log_file_path = os.getenv('LOG_FILE_PATH', '')
    log_file_name = os.getenv('LOG_FILE_NAME', '')
    log_file_extn = os.getenv('LOG_FILE_EXTN', '')

    lines = [MIT_LICENSE, "\n#ifndef ENV_VARS_HPP\n", "#define ENV_VARS_HPP\n\n"]

    if file_logging == 'yes':
        lines.append("#define FILE_LOGGING 1\n")

    if file_size:
        try:
            expr = file_size_to_expression(file_size)
            lines.append(f"#define FILE_SIZE {expr}\n")
        except ValueError as e:
            print(f"Warning: {e}. Skipping FILE_SIZE define.")

    if log_file_path:
        lines.append(f'#define LOG_FILE_PATH {quote_string(log_file_path)}\n')

    if log_file_name:
        lines.append(f'#define LOG_FILE_NAME {quote_string(log_file_name)}\n')

    if log_file_extn:
        ext = log_file_extn
        if not ext.startswith('.'):
            ext = '.' + ext
        lines.append(f'#define LOG_FILE_EXTN {quote_string(ext)}\n')

    lines.append("\n#endif // ENV_VARS_HPP\n")

    # Create include directory if it doesn't exist
    os.makedirs(HEADER_DIR, exist_ok=True)

    with open(HEADER_FILE, 'w') as f:
        f.writelines(lines)

    print(f"Header file '{HEADER_FILE}' generated successfully.")

if __name__ == "__main__":
    main()
