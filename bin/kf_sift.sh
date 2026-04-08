#!/bin/bash
# Kessel Flow: Intelligence Sifter v2.0 (Pillar 02)
# Enhanced with Offensive Vulnerability Signatures

SEARCH_DIR="./research_vault/$(date +%Y-%m-%d)"
REPORT="INTELLIGENCE_REPORT_$(date +%Y-%m-%d).md"

echo -e "--- Kessel Flow Intelligence Brief (Offensive) ---" > $REPORT
echo -e "Date: $(date)\n" >> $REPORT

# 1. THE OPEN DOOR (Status 200/302)
echo "## [TARGETS] Successful Endpoint Access" >> $REPORT
grep -E "HTTP/[0-9.]+ (200|302)" $SEARCH_DIR/*.log | awk '{print $7, $9}' | sort | uniq -c >> $REPORT

# 2. THE BREADCRUMBS (Credential Leaks)
echo -e "\n## [LEAKS] Potential Credentials/Tokens" >> $REPORT
grep -Ei "API_KEY|SECRET|TOKEN|BEARER|PASSWORD" $SEARCH_DIR/*.log >> $REPORT

# 3. [NEW] OFFENSIVE SIGNATURES (SQLi, XSS, LFI)
echo -e "\n## [HUNT] Vulnerability Signatures Detected" >> $REPORT

echo "### SQL Injection Traces" >> $REPORT
grep -Ei "UNION+SELECT|ORDER+BY|SLEEP\(|WAITFOR+DELAY|SELECT+FROM" $SEARCH_DIR/*.log >> $REPORT

echo -e "\n### XSS Payloads" >> $REPORT
grep -Ei "<script|alert\(|onerror=|javascript:|confirm\(" $SEARCH_DIR/*.log >> $REPORT

echo -e "\n### Path Traversal / LFI" >> $REPORT
grep -Ei "\.\.\/|\.\.\\\\|\/etc\/passwd|\/windows\/win.ini|boot.ini" $SEARCH_DIR/*.log >> $REPORT

# 4. THE WALL (Status 403/401)
echo -e "\n## [ZONES] High-Security Clusters" >> $REPORT
grep -E "HTTP/[0-9.]+ (403|401)" $SEARCH_DIR/*.log | awk '{print $7}' | cut -d'/' -f1,2 | sort | uniq -c | sort -nr >> $REPORT

echo "[SUCCESS] Offensive Intelligence Brief generated: $REPORT"
