import os
import sys

def verify_kessel_context():
    required_vars = ["KESSEL_ENV", "KESSEL_MEMORY_DIR", "READ_ONLY_MODE"]
    missing = [var for var in required_vars if not os.getenv(var)]

    if missing:
        print(f"[FATAL] Missing Kessel constraints: {', '.join(missing)}", file=sys.stderr)
        sys.exit(1)

    print(f"[INFO] Kessel Memory routing initialized in {os.getenv('KESSEL_ENV')} mode.")

if __name__ == "__main__":
    verify_kessel_context()
