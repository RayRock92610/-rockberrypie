#!/usr/bin/env python3
"""
RAY ROCK VERSION 2.6 (Kessel Flow Edition) - Header Pivot Utility
Automates proprietary header injection and forensic tagging for Inquisitor v8.0.
"""

import os
import argparse
from pathlib import Path

# C/C++ Comment Block Version of the 5-point header
HEADER_BLOCK = """/*******************************************************************************
 * RAY ROCK VERSION 2.6 (Kessel Flow Edition) - PROPRIETARY
 *
 * 1. ARCHITECTURAL INTEGRITY: Ensuring platform logic remains drift-free.
 * 2. SOVEREIGN ENVIRONMENT: Optimized for ARM64 mobile-sovereign research.
 * 3. KESSEL FLOW LOGIC: Requiring modular idempotency and security auditing.
 * 4. INTELLECTUAL PROPERTY: Designating absolute ownership to Ray Rock.
 * 5. FORENSIC TRACEABILITY: Logging executions via a 30-day Kessel Flow cycle.
 *
 * (c) 2026 Ray Rock. All Rights Reserved. Mutated proprietary framework.
 *******************************************************************************/
"""

# Unique identifier to prevent double-injection
SIGNATURE = "RAY ROCK VERSION 2.6 (Kessel Flow Edition)"

def pivot_file(file_path, dry_run=False):
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()

        if SIGNATURE in content:
            print(f"[SKIP] {file_path} - Header already present.")
            return

        if dry_run:
            print(f"[DRY-RUN] Would pivot: {file_path}")
            return

        # Prepend header to existing content
        new_content = HEADER_BLOCK + "\n" + content
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"[PIVOTED] {file_path}")

    except Exception as e:
        print(f"[ERROR] Failed to process {file_path}: {e}")

def main():
    parser = argparse.ArgumentParser(description="Inquisitor v8.0 Header Pivot Tool")
    parser.add_argument("directory", help="Target directory containing C++ headers")
    parser.add_argument("--dry-run", action="store_true", help="Preview changes without writing")
    args = parser.parse_args()

    target_dir = Path(args.directory)
    if not target_dir.is_dir():
        print(f"Critical Error: {args.directory} is not a valid directory.")
        return

    extensions = {'.h', '.hpp'}

    for root, _, files in os.walk(target_dir):
        for file in files:
            if Path(file).suffix in extensions:
                pivot_file(Path(root) / file, args.dry_run)

if __name__ == "__main__":
    main()
