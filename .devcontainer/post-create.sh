#!/usr/bin/env bash
set -euo pipefail

export DEBIAN_FRONTEND=noninteractive

apt-get update
apt-get install -y --no-install-recommends \
  build-essential \
  cmake \
  ninja-build \
  git \
  pkg-config \
  wget \
  ca-certificates \
  libx11-dev \
  libxrandr-dev \
  libxinerama-dev \
  libxcursor-dev \
  libasound2-dev \
  libjack-jackd2-dev \
  libfreetype6-dev \
  libfontconfig1-dev \
  libxext-dev \
  libxrender-dev \
  libxfixes-dev \
  libxcomposite-dev \
  libgl1-mesa-dev \
  libcurl4-openssl-dev

if [ ! -d /workspaces/juce ]; then
  git clone --depth 1 https://github.com/juce-framework/JUCE.git /workspaces/juce
fi

if [ -f /workspaces/juce/CMakeLists.txt ]; then
  echo "JUCE is available at /workspaces/juce"
fi

if [ -f CMakeLists.txt ]; then
  cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug || true
fi

echo "Codespace setup complete."
