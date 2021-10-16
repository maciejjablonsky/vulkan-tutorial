import sys
import subprocess
import argparse
import os

EXIT_SUCCESS = 0
EXIT_ERROR = -1


class FormattingError(Exception):
    """Raised when any error occured during formatting or style checking"""
    pass


def check_clang_format_available():
    try:
        subprocess.run(["clang-format", "--version"])
    except FileNotFoundError:
        raise FormattingError("clang-format not found!")
    except Exception as e:
        raise FormattingError(repr(e))


def main():
    parser = argparse.ArgumentParser(description='code formatting script')
    parser.add_argument("--inplace", "-i", action="store_true",
                        default=False, help="Turn on inplace formatting")
    parser.add_argument("--path", "-p", type=str,
                        help="Path to directory where recursive search starts", default=".")
    status = EXIT_SUCCESS
    try:
        check_clang_format_available()
        args = parser.parse_args()
        walk_over(args.inplace, args.path)
    except Exception as e:
        print(repr(e))
        status = EXIT_ERROR
    sys.exit(status)


def walk_over(inplace: bool, start_path):
    for root, dirs, files in os.walk(start_path):
        for f in files:
            filepath = os.path.join(root, f)
            format_file(filepath, inplace)


def format_file(filepath, inplace: bool):
    possible_extensions = ['cpp', 'c', 'cc', 'cxx', 'h', 'hpp', 'hxx']
    if any(filepath.endswith(ext) for ext in possible_extensions):
        if inplace:
            format_inplace(filepath)
        else:
            check_style(filepath)


def format_inplace(filepath):
    process = subprocess.run(
        ["clang-format", "--style=file", "-i", filepath], capture_output=True)
    if process.returncode != 0:
        raise FormattingError("Failed to format file inplace!")


def check_style(filepath):
    process = subprocess.run(
        ["clang-format", "--style=file", "--dry-run", filepath], capture_output=True)
    if process.returncode != 0:
        raise FormattingError("Failed to run 'dry run' format check!")
    elif process.stderr != b'':
        raise FormattingError(
            'File is not properly formatted. Either use clang-format manually, via editor plugin, or run this script with "--inplace" flag')


if __name__ == '__main__':
    main()
