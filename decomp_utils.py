import sys
sys.dont_write_bytecode = True

import argparse
import os
import re
import shutil
import subprocess
import tempfile
from pathlib import Path
from colorama import Fore, Style, init

init(autoreset=True)

import build as build_script

# Templates
#
# Functions: FUNCTION: TOY2 0x04CE810
# Stubs: STUB: TOY2 0x04CE810
# Global Vars: GLOBAL: TOY2 0x04CE810
# Bugs: $BUG
# TODO: $TODO

def count_lines():
    build_script.track_process(["cloc", "src"], "CountLines")

def parse_source_files():
    src_main_path = Path("src")
    implemented_functions = {}

    # Track every occurrence so we can report duplicate addresses.
    func_occurrences = {}
    global_occurrences = {}

    func_pattern = re.compile(
        r'//\s*(FUNCTION|STUB):\s*TOY2\s+0x([0-9A-Fa-f]+)'
    )
    global_pattern = re.compile(
        r'//\s*GLOBAL:\s*TOY2\s+0x([0-9A-Fa-f]+)'
    )

    source_files = list(src_main_path.glob("**/*.h")) + list(src_main_path.glob("**/*.cpp"))

    for file_path in source_files:
        try:
            with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
                content = f.read()

            rel_path = str(file_path.relative_to(src_main_path))

            matches = func_pattern.findall(content)

            for kind, address in matches:
                address = address.upper().zfill(8)

                status = "IMPLEMENTED" if kind.upper() == "FUNCTION" else "UNFINISHED"

                implemented_functions[address] = {
                    "status": status,
                    "file": rel_path,
                }
                func_occurrences.setdefault(address, []).append(rel_path)

            for address in global_pattern.findall(content):
                address = address.upper().zfill(8)
                global_occurrences.setdefault(address, []).append(rel_path)

        except Exception as e:
            print(f"Warning: Could not read file {file_path}: {e}")

    duplicate_funcs = {
        address: files
        for address, files in func_occurrences.items()
        if len(files) > 1
    }
    duplicate_globals = {
        address: files
        for address, files in global_occurrences.items()
        if len(files) > 1
    }

    return implemented_functions, duplicate_funcs, duplicate_globals


def report_duplicates(duplicate_funcs, duplicate_globals):
    if not duplicate_funcs and not duplicate_globals:
        return

    print("\n" + "=" * 60)
    print(f"{Fore.RED}DUPLICATE ADDRESS WARNINGS{Style.RESET_ALL}")
    print("=" * 60)

    if duplicate_funcs:
        print(f"{Fore.RED}Duplicate FUNCTION/STUB addresses:{Style.RESET_ALL}")
        for address in sorted(duplicate_funcs):
            files = duplicate_funcs[address]
            print(f"  {address} ({len(files)}x): {', '.join(files)}")

    if duplicate_globals:
        if duplicate_funcs:
            print("-" * 60)
        print(f"{Fore.RED}Duplicate GLOBAL addresses:{Style.RESET_ALL}")
        for address in sorted(duplicate_globals):
            files = duplicate_globals[address]
            print(f"  {address} ({len(files)}x): {', '.join(files)}")

    print("=" * 60)


def parse_functions_map(functions_map_path):
    ida_functions = {}

    with open(functions_map_path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            match = re.match(r'^(?:0x)?([0-9A-Fa-f]{6,8})(?:\s+(.+))?$', line)
            if not match:
                continue

            address = match.group(1).upper().zfill(8)
            func_name = match.group(2).strip() if match.group(2) else ""

            ida_functions[address] = func_name

    return ida_functions


def count_progress(namespace_filter=None, verbose=False):
    functions_map_path = Path("tools/Resources/functions_map.txt")
    if not functions_map_path.exists():
        print("Error: tools/Resources/functions_map.txt not found!")
        return

    try:
        ida_functions = parse_functions_map(functions_map_path)
    except Exception as e:
        print(f"Error reading functions_map.txt: {e}")
        return

    if not ida_functions:
        print("No valid function addresses found in functions_map.txt")
        return

    print("Parsing source files...")
    source_functions, duplicate_funcs, duplicate_globals = parse_source_files()

    report_duplicates(duplicate_funcs, duplicate_globals)

    ida_addresses = set(ida_functions.keys())

    implemented_addresses = set()
    unfinished_addresses = set()

    for address, info in source_functions.items():
        # Only count functions that are actually in the IDA map.
        if address not in ida_addresses:
            continue

        if info["status"] == "IMPLEMENTED":
            implemented_addresses.add(address)
        elif info["status"] == "UNFINISHED":
            unfinished_addresses.add(address)
        else:
            print(f"Found an unknown $FUNC type for {address}")

    if namespace_filter:
        print_namespace_progress(
            namespace_filter,
            ida_functions,
            source_functions,
        )
        return

    total_ida_functions = len(ida_addresses)
    implemented_count = len(implemented_addresses)
    unfinished_count = len(unfinished_addresses)

    not_started_addresses = ida_addresses - implemented_addresses - unfinished_addresses
    not_started_count = len(not_started_addresses)

    print("\n" + "=" * 60)
    print("DECOMPILATION PROGRESS REPORT")
    print("=" * 60)
    print(f"Total functions in IDA:     {total_ida_functions}")
    print(f"Implemented functions:      {implemented_count}")
    print(f"Unfinished functions:       {unfinished_count}")
    print(f"Not started functions:      {not_started_count}")
    print("-" * 60)

    implemented_percentage = (implemented_count / total_ida_functions) * 100
    started_percentage = ((implemented_count + unfinished_count) / total_ida_functions) * 100

    print(f"Implementation progress:    {implemented_percentage:.1f}%")
    print(f"Started progress:           {started_percentage:.1f}%")
    print(f"Implemented vs Overall:     {implemented_count}/{total_ida_functions}")
    print("=" * 60)

    extra_functions = set(source_functions.keys()) - ida_addresses
    if extra_functions and verbose:
        print(f"\nWarning: Found {len(extra_functions)} functions in source files that are not in IDA map:")
        for address in sorted(extra_functions):
            info = source_functions[address]
            print(f"  {address} - {info['file']} [{info['status']}]")
    elif extra_functions:
        print(f"\nWarning: Found {len(extra_functions)} functions in source files that are not in IDA map.")


def print_namespace_progress(namespace, ida_functions, source_functions):
    namespace_prefix = namespace + "::"

    matching_functions = [
        (address, name)
        for address, name in ida_functions.items()
        if name.startswith(namespace_prefix)
    ]

    matching_functions.sort(key=lambda item: item[0])

    print("\n" + "=" * 60)
    print(f"FUNCTIONS FOR {namespace}")
    print("=" * 60)

    if not matching_functions:
        print(f"No functions found in functions_map.txt for namespace: {namespace}")
        print("=" * 60)
        return

    implemented_count = 0
    unfinished_count = 0
    not_started_count = 0

    max_name_len = max(len(func_name) for _, func_name in matching_functions)

    for address, func_name in matching_functions:
        info = source_functions.get(address)

        if info is None:
            status = "NOT_STARTED"
            file_name = ""
            not_started_count += 1
        elif info["status"] == "IMPLEMENTED":
            status = "IMPLEMENTED"
            file_name = info["file"]
            implemented_count += 1
        elif info["status"] == "UNFINISHED":
            status = "UNFINISHED"
            file_name = info["file"]
            unfinished_count += 1
        else:
            status = info["status"]
            file_name = info["file"]

        raw_status = f"[{status}]"

        if status == "IMPLEMENTED":
            coloured_status = f"{Fore.GREEN}{raw_status}{Style.RESET_ALL}"
        elif status == "UNFINISHED":
            coloured_status = f"{Fore.YELLOW}{raw_status}{Style.RESET_ALL}"
        else:
            coloured_status = f"{Fore.RED}{raw_status}{Style.RESET_ALL}"

        status_padding = " " * (14 - len(raw_status))

        print(
            f"0x{address} "
            f"{func_name:<{max_name_len}} "
            f"{coloured_status}{status_padding} "
            f"{file_name}"
        )

    print("-" * 60)
    print(f"Total in namespace:         {len(matching_functions)}")
    print(f"{Fore.GREEN}Implemented:{Style.RESET_ALL}                {implemented_count}")
    print(f"{Fore.YELLOW}Unfinished:{Style.RESET_ALL}                 {unfinished_count}")
    print(f"{Fore.RED}Not started:{Style.RESET_ALL}                {not_started_count}")
    print("=" * 60)

def find_clang_format():
    found = shutil.which("clang-format")

    if found:
        return found

    # the LLVM installer does not always add itself to the PATH
    fallback = Path("C:/Program Files/LLVM/bin/clang-format.exe")

    return str(fallback) if fallback.exists() else None

def format_sources():
    clang_format = find_clang_format()

    if not clang_format:
        print("Error: clang-format was not found, install LLVM or add it to the PATH.")
        return

    files = sorted(str(f) for f in Path("src").rglob("*") if f.suffix in (".cpp", ".h"))

    # the command line is limited in length, so do it in batches
    for i in range(0, len(files), 40):
        result = subprocess.run([clang_format, "-i", "-style=file"] + files[i:i + 40])

        if result.returncode != 0:
            print("Error: clang-format failed.")
            return

    print(f"Formatted {len(files)} files with the root .clang-format")

    # reccmp can not parse an empty body that was put on the line after the signature,
    # the function and the one after it are silently dropped from the report
    pattern = re.compile(r"\)[ \t]*\r?\n[ \t]*\{\}")

    for file in files:
        text = Path(file).read_text(errors="ignore")

        for match in pattern.finditer(text):
            line = text.count("\n", 0, match.start()) + 1
            print(f"{Fore.YELLOW}Warning:{Style.RESET_ALL} {file}:{line} has a '{{}}' body below its signature, reccmp will not see this function. Give the body a comment.")

def generate_progress_image(output_path):
    icon_path = Path("resources/APPICON.ico")
    reccmp_args = ["reccmp-reccmp", "--target", build_script.PROJECT_NAME.upper(), "--silent", "--nolib", "--svg", str(output_path)]

    icon_png = None

    if icon_path.exists():
        try:
            from PIL import Image

            # reccmp only accepts PNG icons, so take the largest image of the .ico
            with Image.open(icon_path) as ico:
                sizes = ico.info.get("sizes") or {ico.size}
                ico.size = max(sizes)
                frame = ico.convert("RGBA")

            handle, icon_png = tempfile.mkstemp(suffix=".png")
            os.close(handle)
            frame.save(icon_png)
            reccmp_args += ["--svg-icon", icon_png]
        except Exception as e:
            print(f"Warning: could not convert {icon_path} to PNG ({e}), generating without icon.")
    else:
        print(f"Warning: {icon_path} not found, generating without icon.")

    try:
        result = subprocess.run(reccmp_args)
    finally:
        if icon_png:
            os.remove(icon_png)

    if result.returncode != 0:
        print("Error: reccmp failed to generate the progress image (is toy2.exe built?).")
        return

    print(f"Progress image written to {output_path}")


def main():
    parser = argparse.ArgumentParser(
        description=f"Decomp utils for {build_script.PROJECT_NAME}."
    )

    parser.add_argument(
        "--count",
        action="store_true",
        help="Count the number of lines in .h and .cpp files.",
    )

    parser.add_argument(
        "--progress",
        nargs="?",
        const=True,
        metavar="NAMESPACE",
        help="Count implemented decomp functions. Optionally filter by namespace.",
    )

    parser.add_argument(
        "--image",
        nargs="?",
        const="progress.svg",
        metavar="FILE",
        help="Generate a progress image (SVG) with the game icon. Defaults to progress.svg.",
    )

    parser.add_argument(
        "--format",
        action="store_true",
        help="Run the root .clang-format on every .cpp and .h file in src.",
    )

    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Shows extra info",
    )

    args = parser.parse_args()

    if args.count:
        count_lines()

    if args.progress:
        namespace_filter = None if args.progress is True else args.progress
        count_progress(namespace_filter, verbose=args.verbose)

    if args.image:
        generate_progress_image(args.image)

    if args.format:
        format_sources()

    if not args.progress and not args.count and not args.image and not args.format:
        print("Enter an option, --count, --progress, --image or --format.")


if __name__ == "__main__":
    main()