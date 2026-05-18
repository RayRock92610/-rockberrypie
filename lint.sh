#!/bin/bash
flake8 drift_detection.py tests/ 2>/dev/null || true
echo "Linting complete."
