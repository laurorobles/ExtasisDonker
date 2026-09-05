#!/bin/bash
echo "============================================================"
echo " Extasis Donker - Linux Automated Installer"
echo "============================================================"

INSTALL_DIR="$(cd "$(dirname "$0")" && pwd)"
VST3_DIR="$HOME/.vst3"
CLAP_DIR="$HOME/.clap"

echo "[1/2] Installing VST3 Plugin to $VST3_DIR..."
mkdir -p "$VST3_DIR"
for vst in "$INSTALL_DIR/Extasis Donker.vst3" "$INSTALL_DIR/ExtasisDonker.vst3" "$INSTALL_DIR/../Extasis Donker.vst3" "$INSTALL_DIR/../ExtasisDonker.vst3"; do
    if [ -d "$vst" ]; then
        cp -R "$vst" "$VST3_DIR/"
        break
    fi
done

echo "[2/2] Installing CLAP Plugin to $CLAP_DIR..."
mkdir -p "$CLAP_DIR"
for clap in "$INSTALL_DIR/Extasis Donker.clap" "$INSTALL_DIR/ExtasisDonker.clap" "$INSTALL_DIR/../Extasis Donker.clap" "$INSTALL_DIR/../ExtasisDonker.clap"; do
    if [ -f "$clap" ]; then
        cp "$clap" "$CLAP_DIR/"
        break
    fi
done

echo ""
echo "============================================================"
echo " Installation Complete! Open your DAW and rescan plugins."
echo "============================================================"
