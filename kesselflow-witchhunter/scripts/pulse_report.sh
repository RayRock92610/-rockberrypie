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

# Count total hash changes and identify the most frequently modified file in a single pass
AWK_OUT=$(awk -F'"path":"' '/HASH_CHANGED/ {
    c++
    if (NF > 1) {
        split($2, a, "\"")
        p[a[1]]++
    }
}
END {
    m = 0
    t = ""
    for (k in p) {
        if (p[k] > m) {
            m = p[k]
            t = k
        }
    }
    printf "%d\n", c
    if (m > 0) {
        printf "%7d %s\n", m, t
    }
}' "$LOG_FILE")

CHANGES=$(echo "$AWK_OUT" | head -n 1)
TOP_DRIFT=$(echo "$AWK_OUT" | tail -n +2)

echo -e "Total Drift Events: ${PEACH}$CHANGES${RESET}"
echo -e "Primary Drift Target: ${PEACH}$TOP_DRIFT${RESET}"
echo "--------------------------------"