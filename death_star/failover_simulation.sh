#!/bin/bash
set -e

echo "🚀 Starting Region Failover Simulation..."
ENDPOINT="https://api.deathstar.empire"

# 1. Start a background monitor to watch traffic
watch -n 1 "curl -s -I $ENDPOINT" &
MONITOR_PID=$!

echo "🔥 Step 1: Simulating Reactor Failure in us-east-1..."
# We scale the ingress controller to 0 to break the health check
kubectl --context prod-us-east-1 scale deployment ingress-nginx-controller -n ingress-nginx --replicas=0

echo "⏳ Step 2: Waiting for Global Accelerator to detect failure (Approx 30s)..."
# Loop until the region header changes from us-east-1 to us-west-2
CURRENT_REGION="us-east-1"
while [ "$CURRENT_REGION" == "us-east-1" ]; do
    CURRENT_REGION=$(curl -s -I $ENDPOINT | grep 'x-amz-region' | awk '{print $2}' | tr -d '\r')
    echo "Current Traffic Region: $CURRENT_REGION"
    sleep 2
done

echo "✅ SUCCESS: Traffic automatically rerouted to $CURRENT_REGION"

echo "🛠️ Step 3: Initiating Auto-Repair (Rebuilding the region)..."
kubectl --context prod-us-east-1 scale deployment ingress-nginx-controller -n ingress-nginx --replicas=3

kill $MONITOR_PID
echo "🌟 Simulation Complete. The Death Star remains operational."