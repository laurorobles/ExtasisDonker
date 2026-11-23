# 📋 RELEASE RULES — EXTASIS DONKER (OPEN SOURCE)

This document defines the version control, documentation, and automated build pipeline rules for official **Extasis Donker** releases.

## 1. Versioning and Metadata
* **Version Structure:** Strict SemVer (`MAJOR.MINOR.PATCH`).
* **CMakeLists.txt:** The version declared in `project(ExtasisDonker VERSION X.Y.Z ...)` must match the Git tag precisely.
* **License:** All releases are distributed under the **GNU General Public License v3.0 (GPLv3)** and must include the `LICENSE` file.

## 2. Required Documentation Files
Every release package must include the following up-to-date files in the project root:
1. `LICENSE`: GNU General Public License v3.0.
2. `README.md`: Executive summary, feature overview, build instructions, and download links.
3. `MANUAL.md`: Complete user manual and MIDI CC mapping guide.
4. `PRESENTATION.md`: Feature showcase overview.
5. `TECHNICAL.md`: Audio engine, DSP specifications, and system requirements.
6. `ARCHITECTURE.md`: End-to-end signal flow and thread-safety architecture.

## 3. Git Workflow & GitHub Actions Release
* **Release Trigger:** Every formal release is initiated by tagging a commit with `vX.Y.Z` (e.g., `v4.0.0`).
* **Multi-Platform CI/CD:** GitHub Actions automatically builds Windows (x64), macOS (Universal), and Linux (x64) binaries simultaneously, packages installers, documentation, and the `LICENSE` file, and publishes the official GitHub Release.
