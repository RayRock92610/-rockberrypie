#!/data/data/com.termux/files/usr/bin/bash
echo "🛰️ Starting Kessel-Flow Core..."

# 1. Trigger Handshake
if [ -f "./bin/kf_node_handshake.sh" ]; then
    bash ./bin/kf_node_handshake.sh
else
    echo "❌ Error: Handshake script not found in ./bin/"
    exit 1
fi

# 2. Launch Cloudflare Tunnel (Z Flip 5 - Port 37511)
echo "🔗 Engaging Cloudflare Bridge..."
nohup cloudflared tunnel --url http://10.0.0.15:37511 > ~/tunnel.log 2>&1 &

echo "✅ Kessel-Flow is pressurized and live."
