# 📋 RELEASE RULES — EXTASIS RECORDS

This document defines the strict version control, documentation, and automated build pipeline rules that must be satisfied before conducting any official public release.

## 1. Versioning and Metadata
* **Version Structure:** Strict SemVer (`MAJOR.MINOR.PATCH`).
* **CMakeLists.txt:** The version declared in `project(... VERSION X.Y.Z)` must match the Git tag precisely.
* **Documentation:** The technical specifications (`TECHNICAL.md`), user manual (`MANUAL.md`), and signal architecture (`ARCHITECTURE.md`) must be updated with the current version and features.

## 2. Required Documentation Files
Every release package must include the following up-to-date Markdown documents in the project root:
1. `README.md`: Executive summary, feature matrix, and download links.
2. `MANUAL.md`: Complete user manual and MIDI CC mapping guide.
3. `PRESENTATION.md`: Commercial presentation pitch and Gumroad purchase links.
4. `TECHNICAL.md`: Audio engine and DSP specifications, system requirements.
5. `ARCHITECTURE.md`: End-to-end signal flow and Mermaid diagrams.

## 3. Git Workflow & GitHub Actions Release
* **Release Trigger:** Every formal release is initiated by tagging a commit with the format `vX.Y.Z` (e.g., `v3.1.0`).
* **Multi-Platform CI/CD:** GitHub Actions automatically builds Windows (x64), macOS (Universal), and Linux (x64) binaries simultaneously, packages installers and documentation, and publishes the official GitHub Release with downloadable zip archives ready for distribution on Gumroad.
