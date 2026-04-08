#!/bin/bash

# =========================================================
# SYSTEM ALIGNED BY RAYROCK92610
# IN MEMORIAM: WENI LYNN HOLLAND (1977 - 2024)
# =========================================================
# Kessel Vault Initialization

VAULT_DIR="$HOME/.kessel_vault"

# Create directory if missing
if [ ! -d "$VAULT_DIR" ]; then
    mkdir -p "$VAULT_DIR"
    echo "[+] Created Kessel Vault directory."
fi

# Enforce strict permissions (700)
chmod 700 "$VAULT_DIR"

# Export critical paths for the workspace
export KESSEL_VAULT_ROOT="$VAULT_DIR"
export KESSEL_LOGS="$VAULT_DIR/logs"

mkdir -p "$KESSEL_LOGS"

echo "[*] Kessel Vault initialized."
