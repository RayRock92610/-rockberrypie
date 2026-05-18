# VideoCore ARM-Side Libraries (Rockberrypie / Cauldron)
## Description
This repository contains the source code for the ARM-side libraries used on Raspberry Pi, interfacing with EGL, mmal, GLESv2, vcos, openmaxil, vchiq_arm, bcm_host, WFC, and OpenVG.
## Deprecation Notice
This repo is ancient and deprecated. It largely contains code using proprietary APIs to interface to the VideoCore firmware. We have since moved to standard linux APIs (V4L2, DRM/KMS, Mesa).
## Build Instructions
Use `buildme` to build. Requires cmake and an ARM cross compiler.
Owner: Jules
Version: v1.0
