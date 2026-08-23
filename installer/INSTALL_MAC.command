#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
PROJECT_DIR="$( cd "$DIR/.." >/dev/null 2>&1 && pwd )"

echo "============================================================"
echo "⚡ EXTASIS DONKER — macOS Automated Installer"
echo "============================================================"

# Check if build directory exists, if not build it
if [ ! -d "$PROJECT_DIR/build" ]; then
    echo "⚙️ Building ExtasisDonker..."
    cd "$PROJECT_DIR"
    cmake -B build -S .
    cmake --build build --config Release -j$(sysctl -n hw.ncpu)
fi

echo "🎹 [1/3] Installing VST3 Plugin to ~/Library/Audio/Plug-Ins/VST3/..."
mkdir -p "$HOME/Library/Audio/Plug-Ins/VST3"
if [ -d "$PROJECT_DIR/build/ExtasisDonker_artefacts/VST3/ExtasisDonker.vst3" ]; then
    rm -rf "$HOME/Library/Audio/Plug-Ins/VST3/ExtasisDonker.vst3"
    cp -R "$PROJECT_DIR/build/ExtasisDonker_artefacts/VST3/ExtasisDonker.vst3" "$HOME/Library/Audio/Plug-Ins/VST3/"
fi

echo "🎵 [2/3] Installing Audio Unit (AU) to ~/Library/Audio/Plug-Ins/Components/..."
mkdir -p "$HOME/Library/Audio/Plug-Ins/Components"
if [ -d "$PROJECT_DIR/build/ExtasisDonker_artefacts/AU/ExtasisDonker.component" ]; then
    rm -rf "$HOME/Library/Audio/Plug-Ins/Components/ExtasisDonker.component"
    cp -R "$PROJECT_DIR/build/ExtasisDonker_artefacts/AU/ExtasisDonker.component" "$HOME/Library/Audio/Plug-Ins/Components/"
fi

echo "💻 [3/3] Installing Standalone App to /Applications/..."
if [ -d "$PROJECT_DIR/build/ExtasisDonker_artefacts/Standalone/ExtasisDonker.app" ]; then
    rm -rf "/Applications/ExtasisDonker.app"
    cp -R "$PROJECT_DIR/build/ExtasisDonker_artefacts/Standalone/ExtasisDonker.app" "/Applications/"
fi

echo ""
echo "============================================================"
echo "✅ Installation Complete! Extasis Donker is ready to use in your DAW."
echo "============================================================"
