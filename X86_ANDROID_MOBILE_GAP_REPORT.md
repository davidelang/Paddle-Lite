# Paddle Lite X86-Android "Mobile Gap" Technical Report & Upstream Guide

**Target:** Android `x86_64` (for Android Studio Emulators)
**Base:** Ubuntu 20.04, Android NDK r20b
**Branch:** `release/v2.14`

## Executive Summary
Paddle Lite's build system assumes a binary paradigm: 
- **ARM** = Mobile / Android (`LITE_ON_TINY_PUBLISH=ON`, NaiveBuffer, no Protobuf, static dependencies).
- **X86** = Desktop / Server (`LITE_ON_TINY_PUBLISH=OFF`, Protobuf, heavy shared frameworks).

When compiling X86 code for an Android target, the build falls into an unsupported "Mobile Gap." The NDK tries to build for Android, but the X86 flags force the inclusion of Desktop-only dependencies, resulting in catastrophic ABI mismatches, missing headers, and undefined symbols. 

We have successfully diagnosed and categorized every structural patch required to bridge this gap. The files are organized in `dev-ai-interaction/paddle-build/patches/` for future upstreaming.

---

## 1. Build System Patches (`patches/build-system/`)

### A. The NDK ABI Conflict (`android.cmake`)
- **Problem:** Paddle Lite hardcoded `set(CMAKE_C_COMPILER clang)`, forcing the build to bypass the Android NDK Toolchain wrappers when compiling `ExternalProject_Add` dependencies like `gflags`. This caused external projects to compile against the host Ubuntu's `libstdc++` while the main library compiled against Android's `libc++`.
- **Fix:** Removed the hardcoded compiler overrides. Explicitly appended `-stdlib=libc++` to `CMAKE_CXX_FLAGS`.

### B. Cross-Compilation Argument Leakage (`postproject.cmake`, `common.cmake`)
- **Problem:** `CROSS_COMPILE_CMAKE_ARGS` (which passes the NDK sysroot down to external projects) was only generated if `LITE_WITH_ARM=ON`. For X86, the arguments were empty, exacerbating the ABI mismatch above.
- **Fix:** Removed the `if(LITE_WITH_ARM)` restriction around `include(postproject)` in `common.cmake`. Hardcoded the absolute path to `CMAKE_SYSROOT` in `postproject.cmake` to guarantee inheritance.

### C. The "Tiny Publish" Override (`CMakeLists.txt`, `lite_CMakeLists.txt`)
- **Problem:** The top-level `CMakeLists.txt` contained a `FATAL_ERROR` blocking `LITE_ON_TINY_PUBLISH=ON` unless `LITE_WITH_ARM` was also true. Furthermore, `lite/CMakeLists.txt` contained hardcoded `cp` commands for full-api objects that don't exist in mobile builds.
- **Fix:** Updated conditions to `(LITE_WITH_ARM OR LITE_WITH_X86)`. Wrapped the full-api `cp` commands in `if(NOT LITE_ON_TINY_PUBLISH)` blocks.

### D. Missing Target Definitions (`api_CMakeLists.txt`)
- **Problem:** In mobile mode, `bundle_full_api` and `paddle_full_api_shared` are skipped. But the final publish logic still depended on them, causing `make` to fail.
- **Fix:** Added `add_custom_target` dummy stubs for these components when `LITE_ON_TINY_PUBLISH` is `ON`. Added explicit `target_link_libraries` to `paddle_light_api_shared` to link `gflags`, `glog`, `xxhash`, and `cblas`.

### E. AVX Feature Detection (`simd.cmake`, `TryRunResults.cmake`)
- **Problem:** `AVX_FOUND` relies on a `TRY_RUN` compiler test. During cross-compilation, CMake cannot run the test, so it defaults to `1` (fail), disabling all AVX kernels on the emulator.
- **Fix:** Pre-populated `TryRunResults.cmake` with `0` (success) for all SIMD features, and aggressively forced `set(AVX_FOUND TRUE)` in `simd.cmake`.

---

## 2. Source Code Patches (`patches/code/`)

### A. Protobuf Leakage in X86 Backend (`fluid/data_type.h`, `fluid/selected_rows.cc`, `fluid/selected_rows.h`)
- **Problem:** X86 math files are shared with the full framework and include `framework.pb.h`. Mobile builds delete the Protobuf source, causing a crash.
- **Fix:** 
    - Wrapped Protobuf includes in `#ifndef LITE_ON_TINY_PUBLISH`.
    - Stubbed the `VarType` enum (found in `framework.pb.h`) with a manual version in `data_type.h` when in mobile mode.
    - Guarded serialization/deserialization functions in `selected_rows` (which rely on Protobuf streams).

### B. Streaming DDimLite Objects (`math/math_function.cc`, `core/dim.h`, `fluid/eigen.h`)
- **Problem:** `CHECK_EQ(output->dims(), in_dims)` attempts to stream a `DDimLite` object into `glog`. The stream operator (`operator<<`) was defined using a custom `STL::ostream` which standard C++ `glog` could not resolve, leading to a massive wall of template errors.
- **Fix:** Added global `std::ostream& operator<<` overloads directly to `lite/core/dim.h`. Replaced `CHECK_EQ` with `CHECK(a == b)` in `math_function.cc` to bypass the stream engine entirely.

### C. MKL / OpenBLAS Fallback Discrepancy (`mklml.h`, `mklml.cc`)
- **Problem:** Android X86 uses OpenBLAS, not Intel MKL. But `mklml.h` attempts to dynamically load MKL function wrappers (`vvsExp`, `vsMul`) using macros that fail to compile if `<mkl.h>` is missing.
- **Fix:** Wrapped the `MKLML_ROUTINE_EACH` macro definitions and declarations in `#ifdef LITE_WITH_MKL` to completely stub them out for Android builds.

---

## 3. External Dependency Patches (`patches/external/`)

### A. OpenBLAS Compiler Strictness (`openblas.cmake`)
- **Problem:** OpenBLAS uses a raw `Makefile` system, bypassing CMake's toolchain flags. The default script cross-compiled it, but failed to pass the Android sysroot and `--target` flag, resulting in OpenBLAS linking against host Ubuntu libraries.
- **Fix:** Bumped the OpenBLAS version from the ancient `v0.2.20` to `v0.3.27`. Hardcoded the `--target=x86_64-none-linux-android21` and `--sysroot` directly into the `OPENBLAS_CC` variable passed to the raw `make` command.

---

## 4. Next Steps for Upstreaming
A developer in a future session can use this categorized patch structure to:
1. Submit the CMake configuration fixes (`android.cmake`, `postproject.cmake`) as a generic "Enable NDK Toolchain for X86 Android" PR.
2. Submit the Protobuf and `DDimLite` guards as a "Support TINY_PUBLISH for X86" PR.
3. Submit the OpenBLAS version bump and compiler target fix independently.
