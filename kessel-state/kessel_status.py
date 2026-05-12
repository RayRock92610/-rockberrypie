#!/usr/bin/env python3
import json
import os
import sys
import time

STATE_FILE = os.path.expanduser("~/.kessel_state.json")

def print_dashboard():
    if not os.path.exists(STATE_FILE):
        print(f"[-] Error: State file not found at {STATE_FILE}")
        print("    Ensure the PROMPT_COMMAND hook is active and you've hit Enter once.")
        sys.exit(1)

    try:
        with open(STATE_FILE, "r") as f:
            state = json.load(f)
    except json.JSONDecodeError:
        print("[-] Error: State file is corrupted or empty.")
        sys.exit(1)

    print("=" * 50)
    print("🧹🌙 Kessel-State Live Dashboard")
    print("=" * 50)

    timestamp = state.get("timestamp", "UNKNOWN")
    print(f"🕒 Last Pulse: {timestamp}")
    print(f"📁 Workspace:  {state.get('pwd', 'UNKNOWN')}")
    print(f"🌿 Git Branch: {state.get('git_branch', 'UNKNOWN')}")

    exit_code = state.get("last_exit_code", -1)
    status_icon = "🟢" if exit_code == 0 else f"🔴 (Code: {exit_code})"
    print(f"🖥️  Last Cmd:   {state.get('last_command', 'UNKNOWN')} {status_icon}")

    print(f"⚙️  Processes:  {state.get('processes', 0)} running")
    print("-" * 50)

    files = state.get("files", "").strip()
    if files:
        file_list = files.split()
        display_files = " ".join(file_list[:5])
        if len(file_list) > 5:
            display_files += f" ... (+{len(file_list)-5} more)"
        print(f"📄 Files:      {display_files}")
    else:
        print("📄 Files:      (Empty Directory)")

    print("=" * 50)

if __name__ == "__main__":
    print_dashboard()
