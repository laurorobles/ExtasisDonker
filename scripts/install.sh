#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
echo "⚡ Building and Installing ExtasisDonker..."

cd "$PROJECT_DIR"
cmake -B build -S .
cmake --build build --config Release -j$(sysctl -n hw.ncpu)

mkdir -p "$HOME/Library/Audio/Plug-Ins/VST3"
mkdir -p "$HOME/Library/Audio/Plug-Ins/Components"

cp -R build/ExtasisDonker_artefacts/VST3/ExtasisDonker.vst3 "$HOME/Library/Audio/Plug-Ins/VST3/"
cp -R build/ExtasisDonker_artefacts/AU/ExtasisDonker.component "$HOME/Library/Audio/Plug-Ins/Components/"

echo "✅ ExtasisDonker VST3 and AU installed successfully into ~/Library/Audio/Plug-Ins/ !"
