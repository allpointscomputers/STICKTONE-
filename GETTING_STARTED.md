# STICKTONE BY WAVEY BEAT — GitHub Cloud Starter

This package prepares the existing StickTone prototype for a GitHub-based cloud workflow. It does not contain a compiled application and does not imply any build has passed.

## A. Create the repository

1. Sign in at https://github.com.
2. Click **+** then **New repository**.
3. Name it `sticktone-by-wavey-beat`.
4. Choose **Private** if you want to keep the source private while developing; choose Public only if you intend to publish it.
5. Do not initialize with a README or license because this project already contains files.
6. Create the repository.

## B. Put the project files into GitHub

GitHub's web upload does not unpack a ZIP automatically. Use the device's built-in file manager to extract this ZIP first (no compiler installation is required), then upload the extracted project files and folders to the new repository. Make sure `.github/workflows/build.yml` is included. If your device cannot extract ZIPs, use a browser-based Codespace or another trusted archive extractor.

Avoid uploading a nested ZIP as the only repository content. The repository root should contain `CMakeLists.txt`, `README.md`, `Source/`, `.github/`, and this guide.

## C. Start a cloud build

1. Open the repository's **Actions** tab.
2. If GitHub asks to enable Actions, enable it for this repository.
3. Choose **StickTone cloud build**.
4. Click **Run workflow**, if shown. A push to the default branch also triggers the workflow.
5. Wait for Linux, Windows, and macOS jobs to finish.
6. Open any failed job and copy the first meaningful compiler/configuration error and surrounding log lines.
7. Download artifacts only from successful jobs.

The workflow installs/uses hosted runner tools, fetches JUCE 8.0.6, configures CMake, and builds the targets supported on each operating system.

## D. Optional browser-based coding environment

GitHub Codespaces can provide a Linux development container with a browser editor and terminal. From the repository page, click **Code → Codespaces → Create codespace**. Availability and included free usage depend on your GitHub account and current quota. Codespaces is Linux-based; use GitHub Actions' Windows and macOS runners for those platform builds.

Official documentation:
- GitHub Actions runners: https://docs.github.com/en/actions/concepts/runners/github-hosted-runners
- GitHub Codespaces: https://docs.github.com/en/codespaces/developing-in-a-codespace/developing-in-a-codespace

## E. Current limitations

- This remains a prototype with synthesized drum voices, not a professional recorded sample library.
- Groove browser, Tap2Find, Song Creator, MIDI export, advanced mixer/effects, custom sample management, and complete preset workflows still need implementation.
- The CI workflow has been added but has not been run in your GitHub account from this package.
- A successful compile does not prove that a plugin behaves correctly in every DAW.
- Signing/notarization and polished installers are later release tasks.
