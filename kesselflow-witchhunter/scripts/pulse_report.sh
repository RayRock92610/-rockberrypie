#!/data/data/com.termux/files/usr/bin/bash
PEACH='\033[38;5;210m'
RESET='\033[0m'
LOG_FILE="$HOME/bug_bounty_workspace/index/events.jsonl"

echo -e "${PEACH}--- KESSEL FLOW PULSE REPORT ---${RESET}"
echo "Generated: $(date)"

if [ ! -f "$LOG_FILE" ]; then
    echo "Error: events.jsonl not found."
    exit 1
fi

# Count total hash changes
CHANGES=$(grep -c "HASH_CHANGED" "$LOG_FILE")
# Identify the most frequently modified file
TOP_DRIFT=$(grep "HASH_CHANGED" "$LOG_FILE" | sed 's/.*"path":"\([^"]*\)".*/\1/' | sort | uniq -c | sort -nr | head -n 1)

echo -e "Total Drift Events: ${PEACH}$CHANGES${RESET}"
echo -e "Primary Drift Target: ${PEACH}$TOP_DRIFT${RESET}"
echo "--------------------------------"