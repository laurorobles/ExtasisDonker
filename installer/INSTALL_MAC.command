#!/bin/bash
echo "============================================================"
echo " Extasis Donker - macOS Automated Installer"
echo "============================================================"

INSTALL_DIR="$(cd "$(dirname "$0")" && pwd)"
VST3_DIR="/Library/Audio/Plug-Ins/VST3"
AU_DIR="/Library/Audio/Plug-Ins/Components"

echo "[1/2] Installing VST3 Plugin to $VST3_DIR..."
sudo mkdir -p "$VST3_DIR"
for vst in "$INSTALL_DIR/Extasis Donker.vst3" "$INSTALL_DIR/ExtasisDonker.vst3" "$INSTALL_DIR/../Extasis Donker.vst3" "$INSTALL_DIR/../ExtasisDonker.vst3"; do
    if [ -d "$vst" ]; then
        echo "Found: $vst"
        sudo cp -R "$vst" "$VST3_DIR/"
        break
    fi
done

echo "[2/2] Installing AU Plugin to $AU_DIR..."
sudo mkdir -p "$AU_DIR"
for au in "$INSTALL_DIR/Extasis Donker.component" "$INSTALL_DIR/ExtasisDonker.component" "$INSTALL_DIR/../Extasis Donker.component" "$INSTALL_DIR/../ExtasisDonker.component"; do
    if [ -d "$au" ]; then
        echo "Found: $au"
        sudo cp -R "$au" "$AU_DIR/"
        break
    fi
done

echo ""
echo "============================================================"
echo " Installation Complete! Open your DAW and rescan plugins."
echo "============================================================"
