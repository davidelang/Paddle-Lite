<!-- Demo: https://github.com/PaddlePaddle/Paddle-Lite/pull/8688 -->
### PR devices
x86

### PR types
Bug fixes

### PR changes
Backends

### Description

**Note on NNAPI:** NNAPI is explicitly disabled for the Android `x86_64` target. Testing on Android Studio emulators revealed that attempts to initialize the NNAPI adapter resulted in immediate crashes. Hard-disabling it ensures the library is stable for emulator-based development.
This PR fixes the catastrophic ABI mismatches and undefined symbol errors that occur when attempting to compile the `x86_64` architecture for an Android target (e.g., for Android Studio Emulators). 

Paddle Lite's build system traditionally assumed X86 implies a Desktop/Server build (`LITE_ON_TINY_PUBLISH=OFF`) and ARM implies a Mobile build (`LITE_ON_TINY_PUBLISH=ON`). When compiling X86 code for an Android target, the build falls into an unsupported "Mobile Gap". The NDK tries to build for Android, but the X86 flags force the inclusion of Desktop-only dependencies, causing the build to fail.

This PR introduces the necessary CMake, source code, and external dependency patches to bridge this gap, enabling `LITE_ON_TINY_PUBLISH=ON` for X86 targets.

**1. Build System Patches**\n* **Robust Packaging (`copy_libs.cmake`):** Added a location-agnostic helper script to ensure library artifacts are correctly collected across different build types.
* **The NDK ABI Conflict (`android.cmake`):** Removed the hardcoded `set(CMAKE_C_COMPILER clang)` overrides that forced external projects to bypass the Android NDK wrappers and compile against the host Ubuntu's `libstdc++`.
* **Cross-Compilation Argument Leakage (`postproject.cmake`, `common.cmake`):** Passed `CROSS_COMPILE_CMAKE_ARGS` down to external projects for X86 targets so they correctly inherit the NDK sysroot.
* **The "Tiny Publish" Override (`CMakeLists.txt`, `lite_CMakeLists.txt`):** Updated conditions to `(LITE_WITH_ARM OR LITE_WITH_X86)` to allow `LITE_ON_TINY_PUBLISH=ON`.
* **AVX Feature Detection (`simd.cmake`):** Forced `AVX_FOUND` to bypass the failing `TRY_RUN` cross-compilation tests so AVX kernels are actually included in the emulator build.

**2. Source Code Patches**
* **Protobuf Leakage (`data_type.h`, `selected_rows.cc`):** Wrapped `framework.pb.h` includes in `#ifndef LITE_ON_TINY_PUBLISH` and stubbed the `VarType` enum, preventing crashes when Protobuf is stripped for mobile.
* **Streaming DDimLite Objects (`math_function.cc`, `dim.h`):** Replaced `CHECK_EQ(output->dims(), in_dims)` with `CHECK(a == b)` to avoid template resolution errors with `glog` and the custom `STL::ostream`.
* **MKL / OpenBLAS Compatibility (`mklml.h`):** Verified and ensured that native Paddle Lite MKL guards are preserved. This allows Desktop Linux builds to maintain MKL support while Android builds safely bypass it.

**3. External Dependency Patches**
* **OpenBLAS Compiler Strictness (`openblas.cmake`):** Hardcoded the Android `--target` and `--sysroot` explicitly into the `OPENBLAS_CC` variable passed to the raw `make` command, ensuring it links against `libc++` instead of host headers.

These changes have been successfully compiled and verified using Ubuntu 20.04 and Android NDK r20b on the `release/v2.14` branch.