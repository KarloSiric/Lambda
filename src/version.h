/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer - Version Information
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 *   This product contains software technology licensed from Id
 *   Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 *   All Rights Reserved.
 *
 *   Use, distribution, and modification of this source code and/or resulting
 *   object code is restricted to non-commercial enhancements to products from
 *   Valve LLC. All other use, distribution, or modification is prohibited
 *   without written permission from Valve LLC.
 *
 * ───────────────────────────────────────────────────────────────────────────
 *   Author: Karlo Siric
 *   Purpose: Semantic versioning and build information
 * ═══════════════════════════════════════════════════════════════════════════
 */

#ifndef VERSION_H
#define VERSION_H

/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   SEMANTIC VERSIONING (https://semver.org/)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 *   Format: MAJOR.MINOR.PATCH-PRERELEASE+BUILD
 * 
 *   MAJOR: Incompatible API changes
 *   MINOR: Add functionality (backward compatible)
 *   PATCH: Bug fixes (backward compatible)
 *   PRERELEASE: alpha, beta, rc (release candidate)
 *   BUILD: Build number or git commit hash
 * 
 *   Examples:
 *     0.1.0-alpha.1     - First alpha release
 *     0.5.0-beta.2      - Second beta release
 *     1.0.0-rc.1        - Release candidate 1
 *     1.0.0             - Stable production release
 *     1.2.3             - Stable with patches
 * ═══════════════════════════════════════════════════════════════════════════
 */

// ═══════════════════════════════════════════════════════════════════════════
//   VERSION COMPONENTS
// ═══════════════════════════════════════════════════════════════════════════

#define HLMV_VERSION_MAJOR          0
#define HLMV_VERSION_MINOR          1
#define HLMV_VERSION_PATCH          0

// Build number (increment with each build)
#define HLMV_VERSION_BUILD          1

// Build timestamp (automatically set by compiler)
#define HLMV_BUILD_DATE             __DATE__
#define HLMV_BUILD_TIME             __TIME__

// ═══════════════════════════════════════════════════════════════════════════
//   PRE-RELEASE CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════
//
//   To create a stable release, comment out HLMV_VERSION_PRERELEASE:
//   // #define HLMV_VERSION_PRERELEASE "alpha.1"
//
//   For development releases, uncomment and set the pre-release identifier:
//   #define HLMV_VERSION_PRERELEASE "alpha.1"
//   #define HLMV_VERSION_PRERELEASE "beta.1"
//   #define HLMV_VERSION_PRERELEASE "rc.1"
//
// ═══════════════════════════════════════════════════════════════════════════

// Current pre-release identifier (comment out for stable releases)
#define HLMV_VERSION_PRERELEASE     "alpha.1"

// ═══════════════════════════════════════════════════════════════════════════
//   GIT INTEGRATION (Set by build system via CMake)
// ═══════════════════════════════════════════════════════════════════════════

#ifndef HLMV_GIT_COMMIT
#define HLMV_GIT_COMMIT             "unknown"
#endif

#ifndef HLMV_GIT_BRANCH
#define HLMV_GIT_BRANCH             "main"
#endif

#ifndef HLMV_GIT_TAG
#define HLMV_GIT_TAG                ""
#endif

// ═══════════════════════════════════════════════════════════════════════════
//   VERSION STRING CONSTRUCTION
// ═══════════════════════════════════════════════════════════════════════════

// Helper macros for stringification
#define HLMV_STRINGIFY(x)           #x
#define HLMV_TOSTRING(x)            HLMV_STRINGIFY(x)

// Base version without pre-release: "0.1.0"
#define HLMV_VERSION_BASE \
    HLMV_TOSTRING(HLMV_VERSION_MAJOR) "." \
    HLMV_TOSTRING(HLMV_VERSION_MINOR) "." \
    HLMV_TOSTRING(HLMV_VERSION_PATCH)

// Standard version string (with or without pre-release)
#ifdef HLMV_VERSION_PRERELEASE
    // Pre-release version: "0.1.0-alpha.1"
    #define HLMV_VERSION_STRING     HLMV_VERSION_BASE "-" HLMV_VERSION_PRERELEASE
#else
    // Stable version: "0.1.0"
    #define HLMV_VERSION_STRING     HLMV_VERSION_BASE
#endif

// Full version with build metadata: "0.1.0-alpha.1+build.1"
#define HLMV_VERSION_FULL \
    HLMV_VERSION_STRING "+build." HLMV_TOSTRING(HLMV_VERSION_BUILD)

// Short display version: "v0.1.0-alpha.1"
#define HLMV_VERSION_SHORT          "v" HLMV_VERSION_STRING

// Long display version: "Half-Life Model Viewer v0.1.0-alpha.1 (build 1)"
#define HLMV_VERSION_LONG \
    "Half-Life Model Viewer " HLMV_VERSION_SHORT " (build " HLMV_TOSTRING(HLMV_VERSION_BUILD) ")"

// ═══════════════════════════════════════════════════════════════════════════
//   API VERSION (For plugin compatibility)
// ═══════════════════════════════════════════════════════════════════════════

// Increment when breaking changes are made to plugin API
#define HLMV_API_VERSION            1

// ═══════════════════════════════════════════════════════════════════════════
//   FEATURE FLAGS (Capabilities of this build)
// ═══════════════════════════════════════════════════════════════════════════

#define HLMV_HAS_RENDERING          1   // Basic OpenGL rendering
#define HLMV_HAS_ANIMATION          1   // Animation playback
#define HLMV_HAS_TEXTURES           1   // Texture support
#define HLMV_HAS_BONES              1   // Bone system
#define HLMV_HAS_SEQUENCES          1   // Sequence groups
#define HLMV_HAS_DUMP               1   // Model dump feature

// Features not yet implemented
#define HLMV_HAS_GUI                0   // Qt GUI interface
#define HLMV_HAS_EXPORT             0   // Model export (OBJ, FBX, etc.)
#define HLMV_HAS_EDITING            0   // Model editing
#define HLMV_HAS_COMPILER           0   // QC compiler integration
#define HLMV_HAS_PLUGINS            0   // Plugin system

// ═══════════════════════════════════════════════════════════════════════════
//   PROJECT INFORMATION
// ═══════════════════════════════════════════════════════════════════════════

#define HLMV_PROJECT_NAME           "Half-Life Model Viewer"
#define HLMV_PROJECT_CODENAME       "Lambda"
#define HLMV_PROJECT_DESCRIPTION    "GoldSrc Studio Model Format Viewer"
#define HLMV_PROJECT_AUTHOR         "Karlo Siric"
#define HLMV_PROJECT_LICENSE        "Valve SDK License"
#define HLMV_PROJECT_URL            "https://github.com/karlosiric/ModelViewer"

// ═══════════════════════════════════════════════════════════════════════════
//   COPYRIGHT INFORMATION
// ═══════════════════════════════════════════════════════════════════════════

#define HLMV_COPYRIGHT_YEAR         "2025"
#define HLMV_COPYRIGHT_HOLDER       "Karlo Siric"

#define HLMV_VALVE_COPYRIGHT \
    "Copyright (c) 1996-2002, Valve LLC. All rights reserved.\n" \
    "This product contains software technology licensed from Id\n" \
    "Software, Inc. (\"Id Technology\"). Id Technology (c) 1996\n" \
    "Id Software, Inc. All Rights Reserved."

// ═══════════════════════════════════════════════════════════════════════════
//   VERSION COMPARISON HELPERS
// ═══════════════════════════════════════════════════════════════════════════

// Create a numeric version for comparison: 0x00MMNNPP (Major.Minor.Patch)
#define HLMV_VERSION_NUMBER \
    ((HLMV_VERSION_MAJOR << 16) | (HLMV_VERSION_MINOR << 8) | HLMV_VERSION_PATCH)

// Check if version is at least X.Y.Z
#define HLMV_VERSION_AT_LEAST(major, minor, patch) \
    (HLMV_VERSION_NUMBER >= (((major) << 16) | ((minor) << 8) | (patch)))

// ═══════════════════════════════════════════════════════════════════════════
//   BUILD CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════

// Build type detection
#if defined(NDEBUG) || defined(_NDEBUG)
    #define HLMV_BUILD_TYPE         "Release"
    #define HLMV_IS_DEBUG_BUILD     0
    #define HLMV_IS_RELEASE_BUILD   1
#else
    #define HLMV_BUILD_TYPE         "Debug"
    #define HLMV_IS_DEBUG_BUILD     1
    #define HLMV_IS_RELEASE_BUILD   0
#endif

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define HLMV_PLATFORM           "Windows"
    #define HLMV_PLATFORM_WINDOWS   1
#elif defined(__APPLE__) && defined(__MACH__)
    #define HLMV_PLATFORM           "macOS"
    #define HLMV_PLATFORM_MACOS     1
#elif defined(__linux__)
    #define HLMV_PLATFORM           "Linux"
    #define HLMV_PLATFORM_LINUX     1
#else
    #define HLMV_PLATFORM           "Unknown"
    #define HLMV_PLATFORM_UNKNOWN   1
#endif

// Compiler detection
#if defined(__clang__)
    #define HLMV_COMPILER           "Clang " __clang_version__
#elif defined(__GNUC__)
    #define HLMV_COMPILER           "GCC " __VERSION__
#elif defined(_MSC_VER)
    #define HLMV_COMPILER           "MSVC"
#else
    #define HLMV_COMPILER           "Unknown"
#endif

// Architecture detectio
#if defined(__x86_64__) || defined(_M_X64)
    #define HLMV_ARCH               "x86_64"
    #define HLMV_ARCH_64BIT         1
#elif defined(__i386__) || defined(_M_IX86)
    #define HLMV_ARCH               "x86"
    #define HLMV_ARCH_32BIT         1
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define HLMV_ARCH               "ARM64"
    #define HLMV_ARCH_64BIT         1
#elif defined(__arm__) || defined(_M_ARM)
    #define HLMV_ARCH               "ARM"
    #define HLMV_ARCH_32BIT         1
#else
    #define HLMV_ARCH               "Unknown"
#endif

// ═══════════════════════════════════════════════════════════════════════════
//   RUNTIME VERSION FUNCTIONS (Optional C functions)
// ═══════════════════════════════════════════════════════════════════════════

#ifdef __cplusplus
extern "C" {
#endif

// Get version components
static inline int hlmv_get_version_major(void) { return HLMV_VERSION_MAJOR; }
static inline int hlmv_get_version_minor(void) { return HLMV_VERSION_MINOR; }
static inline int hlmv_get_version_patch(void) { return HLMV_VERSION_PATCH; }
static inline int hlmv_get_version_build(void) { return HLMV_VERSION_BUILD; }

// Get version strings
static inline const char* hlmv_get_version_string(void) { return HLMV_VERSION_STRING; }
static inline const char* hlmv_get_version_full(void) { return HLMV_VERSION_FULL; }
static inline const char* hlmv_get_version_short(void) { return HLMV_VERSION_SHORT; }
static inline const char* hlmv_get_version_long(void) { return HLMV_VERSION_LONG; }

// Get build information
static inline const char* hlmv_get_build_date(void) { return HLMV_BUILD_DATE; }
static inline const char* hlmv_get_build_time(void) { return HLMV_BUILD_TIME; }
static inline const char* hlmv_get_build_type(void) { return HLMV_BUILD_TYPE; }

// Get git information
static inline const char* hlmv_get_git_commit(void) { return HLMV_GIT_COMMIT; }
static inline const char* hlmv_get_git_branch(void) { return HLMV_GIT_BRANCH; }

// Get platform information
static inline const char* hlmv_get_platform(void) { return HLMV_PLATFORM; }
static inline const char* hlmv_get_compiler(void) { return HLMV_COMPILER; }
static inline const char* hlmv_get_architecture(void) { return HLMV_ARCH; }

// Check features
static inline int hlmv_has_gui(void) { return HLMV_HAS_GUI; }
static inline int hlmv_has_export(void) { return HLMV_HAS_EXPORT; }
static inline int hlmv_has_editing(void) { return HLMV_HAS_EDITING; }

#ifdef __cplusplus
}
#endif

#endif // VERSION_H
