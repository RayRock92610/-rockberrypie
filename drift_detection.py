import os
import json
import hashlib
import fnmatch
import argparse
import sys
import concurrent.futures

# CONFIGURATION DEFAULTS
BASELINE_FILE = os.environ.get("K_BASELINE", "baseline.json")
CONFIG_FILE = "config.ini"
BUFFER_SIZE = 1048576

def get_file_hash(filepath):
    try:
        with open(filepath, "rb") as f:
            # ⚡ Bolt: Use hashlib.file_digest (Python 3.11+) to execute the read-and-update
            # loop entirely in C/OpenSSL, bypassing Python buffer allocations.
            if hasattr(hashlib, "file_digest"):
                return hashlib.file_digest(f, "sha256").hexdigest()

            # Fallback for older runtimes
            sha256_hash = hashlib.sha256()
            size = os.path.getsize(filepath)
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
    if isinstance(exclusions, re.Pattern):
        return bool(exclusions.match(os.path.normcase(path)) or exclusions.match(os.path.normcase(name)))

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

def _get_compiled_exclusions(exclusions):
    patterns = exclusions if isinstance(exclusions, tuple) else tuple(exclusions.get('dirs', []) + exclusions.get('files', []))
    if not patterns:
        return None
    if patterns not in _CACHE:
        regex_str = '|'.join([fnmatch.translate(os.path.normcase(p)) for p in patterns])
        _CACHE[patterns] = re.compile(regex_str)
    return _CACHE[patterns]

def create_baseline(directory, exclusions):
    baseline = {}
    compiled_exclusions = _get_compiled_exclusions(exclusions)

    # ⚡ Bolt: Use a ThreadPoolExecutor to parallelize the hashing I/O bottleneck
    with concurrent.futures.ThreadPoolExecutor() as executor:
        future_to_path = {}
        for root, dirs, files in os.walk(directory):
            # In-place modification of dirs to skip excluded subtrees
            if compiled_exclusions:
                dirs[:] = [d for d in dirs if not is_excluded(os.path.join(root, d), d, compiled_exclusions)]

            # ⚡ Bolt: Calculate relative path once per directory instead of per file
            # to avoid significant path manipulation overhead.
            root_rel = os.path.relpath(root, directory) if root != directory else ""

            for f in files:
                full_path = os.path.join(root, f)
                if compiled_exclusions and is_excluded(full_path, f, compiled_exclusions): continue

                rel_path = os.path.join(root_rel, f) if root_rel else f

                # ⚡ Bolt: Add stat information to the baseline
                stat = os.stat(full_path)
                future = executor.submit(get_file_hash, full_path)
                future_to_path[future] = (rel_path, stat.st_size, stat.st_mtime)

        for future in concurrent.futures.as_completed(future_to_path):
            rel_path, size, mtime = future_to_path[future]
            f_hash = future.result()
            if f_hash: baseline[rel_path] = {"hash": f_hash, "size": size, "mtime": mtime}

    with open(BASELINE_FILE, "w") as f:
        json.dump(baseline, f, indent=4)
    return len(baseline)

def check_integrity(directory, exclusions):
    if not os.path.exists(BASELINE_FILE): return None, "Baseline missing"

    with open(BASELINE_FILE, "r") as f:
        baseline = json.load(f)

    # Convert old baseline format to new format
    for k, v in baseline.items():
        if isinstance(v, str):
            baseline[k] = {"hash": v, "size": -1, "mtime": -1}

    current_state = {}
    compiled_exclusions = _get_compiled_exclusions(exclusions)

    # ⚡ Bolt: Use a ThreadPoolExecutor to parallelize the hashing I/O bottleneck
    with concurrent.futures.ThreadPoolExecutor() as executor:
        future_to_path = {}
        for root, dirs, files in os.walk(directory):
            # In-place modification of dirs to skip excluded subtrees
            if compiled_exclusions:
                dirs[:] = [d for d in dirs if not is_excluded(os.path.join(root, d), d, compiled_exclusions)]

            # ⚡ Bolt: Calculate relative path once per directory instead of per file
            root_rel = os.path.relpath(root, directory) if root != directory else ""

            for f in files:
                full_path = os.path.join(root, f)
                if compiled_exclusions and is_excluded(full_path, f, compiled_exclusions): continue
                rel_path = os.path.join(root_rel, f) if root_rel else f

                # We don't need to hash if it's a new file, it will be added to new files list
                if rel_path in baseline:
                    baseline_info = baseline[rel_path]
                    stat = os.stat(full_path)

                    # ⚡ Bolt: Fast-path optimization
                    # Bypass expensive file hashing if the file size and mtime are unchanged
                    if stat.st_size == baseline_info.get("size") and stat.st_mtime == baseline_info.get("mtime"):
                        current_state[rel_path] = baseline_info["hash"]
                    else:
                        future = executor.submit(get_file_hash, full_path)
                        future_to_path[future] = rel_path
                else:
                    current_state[rel_path] = None # Or just track the key

        for future in concurrent.futures.as_completed(future_to_path):
            rel_path = future_to_path[future]
            f_hash = future.result()
            if f_hash: current_state[rel_path] = f_hash

    b_keys, c_keys = baseline.keys(), current_state.keys()

    return {
        "new": list(c_keys - b_keys),
        "deleted": list(b_keys - c_keys),
        "modified": [f for f in b_keys & c_keys if baseline[f]["hash"] != current_state.get(f)]
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
