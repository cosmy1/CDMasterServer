# Crashday 1.2 Master Server Source Code

This repository contains the source code for Crashday 1.2 Master Server.

The purpose of this release is historical preservation, educational use, and enabling community-hosted master servers.

This source code is released under the terms of the **GNU General Public License v3.0 (GPL-3.0)**. See the `LICENSE` file for the complete license text.

# Source Code Status

The repository contains the complete master server source code, except for the following proprietary libraries whose source code is no longer available:

- `RakNet`
- `RakCoverDLL`

To allow the project to link with modern toolchains, an x86 import `.lib` was generated from `RakCoverDLL.dll` exports using the Visual Studio 2022 C++ toolchain.

# Building

A Visual Studio 2022 solution is provided inside Source folder for Win32 (x86) builds.

The project is limited to 32-bit because it depends on the original `RakNet` and `RakCoverDLL` binaries, which are available only in x86 form.

## Acknowledgements

Special thanks to Robert Clemens for granting permission to release this source code under an open-source license for the benefit of the community.
