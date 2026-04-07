#!/bin/bash
# Kessel Flow: Intelligence Sifter (Pillar 01)
# Parses exfiltrated logs for High-Value Targets.

SEARCH_DIR="./research_vault/$(date +%Y-%m-%d)"
REPORT="INTELLIGENCE_REPORT_$(date +%Y-%m-%d).md"

echo -e "--- Kessel Flow Intelligence Brief ---" > $REPORT
echo -e "Date: $(date)\n" >> $REPORT

# 1. THE OPEN DOOR (Status 200/302)
echo "## [TARGETS] Successful Endpoint Access (200/302)" >> $REPORT
grep -E "HTTP/[0-9.]+ (200|302)" $SEARCH_DIR/*.log | awk '{print $7, $9}' | sort | uniq -c >> $REPORT

# 2. THE BREADCRUMBS (Credential Leaks)
echo -e "\n## [LEAKS] Potential Credentials/Tokens" >> $REPORT
grep -Ei "API_KEY|SECRET|TOKEN|BEARER|PASSWORD" $SEARCH_DIR/*.log >> $REPORT

# 3. THE WALL (Status 403/401)
echo -e "\n## [ZONES] High-Security Clusters (403/401)" >> $REPORT
grep -E "HTTP/[0-9.]+ (403|401)" $SEARCH_DIR/*.log | awk '{print $7}' | cut -d'/' -f1,2 | sort | uniq -c | sort -nr >> $REPORT

echo "[SUCCESS] Intelligence Brief generated: $REPORT"
