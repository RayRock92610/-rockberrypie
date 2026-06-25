import os
import json
import hashlib
import fnmatch
import argparse
import sys

# CONFIGURATION DEFAULTS
BASELINE_FILE = os.environ.get("K_BASELINE", "baseline.json")
CONFIG_FILE = "config.ini"
BUFFER_SIZE = 1048576

def get_file_hash(filepath):
    sha256_hash = hashlib.sha256()
    try:
        size = os.path.getsize(filepath)
        with open(filepath, "rb") as f:
            # ⚡ Bolt: Read small files entirely into memory to eliminate the loop overhead
            # associated with chunked reading.
            if size <= BUFFER_SIZE:
                sha256_hash.update(f.read())
            else:
                while data := f.read(BUFFER_SIZE):
                    sha256_hash.update(data)
        return sha256_hash.hexdigest()
    except IOError:
        return None

import re
# ⚡ Bolt: Cache compiled regex patterns keyed by the exclusion tuple to
# avoid repetitive compilation and achieve O(1) matching time after initialization.
_CACHE = {}

def is_excluded(path, name, exclusions):
    # ⚡ Bolt: Check if exclusions is already a tuple to avoid repeated concatenation and conversion
    patterns = exclusions if isinstance(exclusions, tuple) else tuple(exclusions.get('dirs', []) + exclusions.get('files', []))
    if not patterns:
        return False

    if patterns not in _CACHE:
        # ⚡ Bolt: Pre-compile multiple glob patterns into a single regular expression.
        # This prevents the O(N*M) overhead of iteratively calling fnmatch.fnmatch()
        # inside deep os.walk directory traversals.
        regex_str = '|'.join([fnmatch.translate(os.path.normcase(p)) for p in patterns])
        _CACHE[patterns] = re.compile(regex_str)

    regex = _CACHE[patterns]


    return bool(regex.match(os.path.normcase(path)) or regex.match(os.path.normcase(name)))

def create_baseline(directory, exclusions):
    baseline = {}
    exclusions_tuple = tuple(exclusions.get('dirs', []) + exclusions.get('files', []))
    for root, dirs, files in os.walk(directory):
        # In-place modification of dirs to skip excluded subtrees
        dirs[:] = [d for d in dirs if not is_excluded(os.path.join(root, d), d, exclusions_tuple)]

        # ⚡ Bolt: Calculate relative path once per directory instead of per file
        # to avoid significant path manipulation overhead.
        root_rel = os.path.relpath(root, directory) if root != directory else ""

        for f in files:
            full_path = os.path.join(root, f)
            if is_excluded(full_path, f, exclusions_tuple): continue

            rel_path = os.path.join(root_rel, f) if root_rel else f
            f_hash = get_file_hash(full_path)
            if f_hash: baseline[rel_path] = f_hash

    with open(BASELINE_FILE, "w") as f:
        json.dump(baseline, f, indent=4)
    return len(baseline)

def check_integrity(directory, exclusions):
    if not os.path.exists(BASELINE_FILE): return None, "Baseline missing"

    with open(BASELINE_FILE, "r") as f:
        baseline = json.load(f)

    current_state = {}
    exclusions_tuple = tuple(exclusions.get('dirs', []) + exclusions.get('files', []))
    for root, dirs, files in os.walk(directory):
        # In-place modification of dirs to skip excluded subtrees
        dirs[:] = [d for d in dirs if not is_excluded(os.path.join(root, d), d, exclusions_tuple)]

        # ⚡ Bolt: Calculate relative path once per directory instead of per file
        root_rel = os.path.relpath(root, directory) if root != directory else ""

        for f in files:
            full_path = os.path.join(root, f)
            if is_excluded(full_path, f, exclusions_tuple): continue
            rel_path = os.path.join(root_rel, f) if root_rel else f

            # We don't need to hash if it's a new file, it will be added to new files list
            if rel_path in baseline:
                f_hash = get_file_hash(full_path)
                if f_hash:
                    current_state[rel_path] = f_hash
            else:
                current_state[rel_path] = None # Or just track the key

    b_set, c_set = set(baseline.keys()), set(current_state.keys())

    return {
        "new": list(c_set - b_set),
        "deleted": list(b_set - c_set),
        "modified": [f for f in b_set & c_set if baseline[f] != current_state.get(f)]
    }, None

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="KesselFlow Drift Detection")
    parser.add_argument("--create-baseline", metavar="DIR", help="Create baseline for directory")
    parser.add_argument("--check-integrity", metavar="DIR", help="Check integrity of directory")
    args = parser.parse_args()

    # Minimal default exclusions for testing
    exclusions = {"dirs": [".git", "__pycache__"], "files": ["baseline.json", "*.log", "config.ini"]}

    if args.create_baseline:
        count = create_baseline(args.create_baseline, exclusions)
        print(f"Baseline created with {count} files.")
    elif args.check_integrity:
        result, err = check_integrity(args.check_integrity, exclusions)
        if err:
            print(f"Error: {err}", file=sys.stderr)
            sys.exit(1)
        print(json.dumps(result, indent=2))
    else:
        parser.print_help()
