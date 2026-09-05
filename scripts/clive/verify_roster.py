import json
import os
import sys

def verify_dispatch():
    config_path = "config/clive_roster.json"
    if not os.path.exists(config_path):
        print("[FAIL] clive_roster.json not found.")
        sys.exit(1)

    with open(config_path, "r") as f:
        data = json.load(f)

    roster = data.get("primary_agents", {})
    print(f"[OK] Roster Loaded: {data.get('roster_name')} ({data.get('total_agents')} agents total)")
    print("-" * 50)
    
    for agent, info in roster.items():
        fallback = info.get("fallback")
        if fallback not in roster:
            print(f"[WARN] Agent '{agent}' fallback '{fallback}' is outside primary mapped roster.")
        else:
            print(f"[VALID] {agent.upper():<10} | Role: {info['role']:<22} | Fallback -> {fallback.upper()}")

    print("-" * 50)
    print("[SUCCESS] Clive Persona Roster dispatch routes verified.")

if __name__ == "__main__":
    verify_dispatch()
