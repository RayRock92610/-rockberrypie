import os
import json
import hashlib
import fnmatch
import configparser
import argparse
import sys
import re
from datetime import datetime

# CONFIGURATION DEFAULTS
BASELINE_FILE = os.environ.get("K_BASELINE", "baseline.json")
CONFIG_FILE = "config.ini"
BUFFER_SIZE = 65536

def get_file_hash(filepath):
    sha256_hash = hashlib.sha256()
    try:
        with open(filepath, "rb") as f:
            while data := f.read(BUFFER_SIZE):
                sha256_hash.update(data)
        return sha256_hash.hexdigest()
    except IOError:
        return None

def build_exclusion_regex(exclusions):
    patterns = exclusions.get('dirs', []) + exclusions.get('files', [])
    if not patterns: return None
    # ⚡ Bolt: Compile glob patterns into a single regex.
    # fnmatch.fnmatch loop is O(N*M), whereas a single regex match is much faster for deep directory walks.
    regexes = [fnmatch.translate(os.path.normcase(p)) for p in patterns]
    return re.compile('|'.join(regexes))

def is_excluded(path, exclusion_regex):
    if not exclusion_regex: return False
    # Use os.path.normcase to ensure cross-platform behavior parity with fnmatch.fnmatch
    path_norm = os.path.normcase(path)
    name_norm = os.path.normcase(os.path.basename(path))
    return bool(exclusion_regex.match(path_norm) or exclusion_regex.match(name_norm))

def create_baseline(directory, exclusions):
    baseline = {}
    exclusion_regex = build_exclusion_regex(exclusions)
    for root, dirs, files in os.walk(directory):
        # In-place modification of dirs to skip excluded subtrees
        dirs[:] = [d for d in dirs if not is_excluded(os.path.join(root, d), exclusion_regex)]
        for f in files:
            full_path = os.path.join(root, f)
            if is_excluded(full_path, exclusion_regex): continue

            rel_path = os.path.relpath(full_path, directory)
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
    exclusion_regex = build_exclusion_regex(exclusions)
    for root, dirs, files in os.walk(directory):
        dirs[:] = [d for d in dirs if not is_excluded(os.path.join(root, d), exclusion_regex)]
        for f in files:
            full_path = os.path.join(root, f)
            if is_excluded(full_path, exclusion_regex): continue
            rel_path = os.path.relpath(full_path, directory)
            f_hash = get_file_hash(full_path)
            if f_hash: current_state[rel_path] = f_hash

    b_set, c_set = set(baseline.keys()), set(current_state.keys())

    return {
        "new": list(c_set - b_set),
        "deleted": list(b_set - c_set),
        "modified": [f for f in b_set & c_set if baseline[f] != current_state[f]]
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
