#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
echo "⚡ Building and Installing Extasis Donker (Free & Open Source)..."

cd "$PROJECT_DIR"
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j$(sysctl -n hw.ncpu)

echo "✅ Extasis Donker VST3 and AU installed successfully into ~/Library/Audio/Plug-Ins/ !"
