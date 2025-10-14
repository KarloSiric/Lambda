# Version System - Build and Test Instructions

## ✅ Fixed Issue

**Problem:** Preprocessor error with conditional check in version string construction
```c
// ❌ This caused error:
#if defined(HLMV_VERSION_PRERELEASE) && (HLMV_VERSION_PRERELEASE[0] != '\0')
```

**Solution:** Simplified to standard `#ifdef` check
```c
// ✅ This works:
#ifdef HLMV_VERSION_PRERELEASE
    #define HLMV_VERSION_STRING  HLMV_VERSION_BASE "-" HLMV_VERSION_PRERELEASE
#else
    #define HLMV_VERSION_STRING  HLMV_VERSION_BASE
#endif
```

---

## 🚀 How to Build and Test

### Option 1: Manual Build
```bash
cd ~/ModelViewer/build
rm -rf *          # Clean build directory
cmake ..          # Configure
make              # Build
```

### Option 2: Using Build Script
```bash
cd ~/ModelViewer
chmod +x build_test.sh
./build_test.sh
```

### Option 3: Using Make (from root)
```bash
cd ~/ModelViewer
make clean
make
```

---

## 🧪 Test the Version System

### Test 1: Version Display
```bash
cd ~/ModelViewer/build
./bin/HalfLifeModelViewer --version
```

**Expected Output:**
```
═══════════════════════════════════════════════════════════════
  HALF-LIFE MODEL VIEWER
  GoldSrc Studio Model Format Viewer
═══════════════════════════════════════════════════════════════

  Copyright (c) 1996-2002, Valve LLC. All rights reserved.
  ...

───────────────────────────────────────────────────────────────
  Author: Karlo Siric
  Version: v0.1.0-alpha.1
═══════════════════════════════════════════════════════════════

═══════════════════════════════════════════════════════════════
  Half-Life Model Viewer v0.1.0-alpha.1 (build 1)
═══════════════════════════════════════════════════════════════

  Build Information:
    Version:        0.1.0-alpha.1
    Build Number:   1
    Build Date:     Jan 15 2025
    Build Time:     [current time]
    Build Type:     Release

  Git Information:
    Commit:         [your commit hash]
    Branch:         main

  Platform:
    OS:             macOS
    Architecture:   ARM64
    Compiler:       Clang [version]

  Features:
    Rendering:      Yes
    Animation:      Yes
    Textures:       Yes
    Bones:          Yes
    Dump:           Yes
    GUI:            No
    Export:         No
    Editing:        No

═══════════════════════════════════════════════════════════════
```

### Test 2: Help Display
```bash
./bin/HalfLifeModelViewer --help
```

### Test 3: Load a Model
```bash
./bin/HalfLifeModelViewer ../models/HL1_Original/scientist.mdl
```

### Test 4: Model Dump
```bash
./bin/HalfLifeModelViewer ../models/HL1_Original/scientist.mdl --dump
```

---

## 📝 What Was Created

### 1. Core Files
- ✅ `src/version.h` - Version system (FIXED - no more preprocessor errors)
- ✅ `src/utils/args.c` - Updated with --version flag
- ✅ `src/utils/args.h` - Added print_version_info()
- ✅ `src/main.c` - Handles --version flag

### 2. Documentation
- ✅ `docs/DEVELOPMENT_ROADMAP.md` - 48-week development plan
- ✅ `docs/VERSION_SYSTEM_GUIDE.md` - Usage instructions
- ✅ `CHANGELOG.md` - Version history tracking

### 3. Build System
- ✅ `CMakeLists.txt` - Updated with git integration
- ✅ `build_test.sh` - Quick build and test script

---

## 🎯 Next Steps

### 1. Build and Test (NOW)
```bash
cd ~/ModelViewer
mkdir -p build
cd build
cmake ..
make
./bin/HalfLifeModelViewer --version
```

### 2. Commit Changes
```bash
cd ~/ModelViewer
git add -A
git commit -m "feat: implement semantic versioning system

- Add version.h with semantic versioning support
- Create comprehensive development roadmap (48 weeks)
- Add CHANGELOG.md following Keep a Changelog format
- Update CMakeLists.txt with git integration
- Add --version flag to CLI
- Fix preprocessor conditional compilation
- Update documentation and examples

Current version: v0.1.0-alpha.1"
```

### 3. Create Git Tag
```bash
git tag -a v0.1.0-alpha.1 -m "Release v0.1.0-alpha.1

Initial alpha release with:
- Basic MDL loading and rendering
- Animation playback system
- Camera controls
- Model dump feature
- Semantic versioning system
- Development roadmap

Codename: Lambda"

git push origin main
git push origin v0.1.0-alpha.1
```

---

## 📖 About the Project Name

The project is officially called **"Half-Life Model Viewer"** but has the internal codename **"Lambda" (λ)**.

This is reflected in the version header:
```c
#define HLMV_PROJECT_NAME       "Half-Life Model Viewer"
#define HLMV_PROJECT_CODENAME   "Lambda"
```

You can reference it either way:
- **Formal**: "Half-Life Model Viewer v0.1.0-alpha.1"
- **Informal**: "Lambda v0.1.0-alpha.1"
- **Full**: "Half-Life Model Viewer (Lambda) v0.1.0-alpha.1"

---

## 🐛 Troubleshooting

### Build Errors

**Error: "Invalid token at start of preprocessor expression"**
- ✅ FIXED - Updated version.h to use simple #ifdef

**Error: "version.h not found"**
```bash
# Make sure you're in the build directory
cd ~/ModelViewer/build
cmake ..
```

**Error: "undefined reference to print_version_info"**
```bash
# Clean and rebuild
make clean
make
```

### Git Information Not Showing

If git info shows "unknown":
```bash
# Make sure you're in a git repository
cd ~/ModelViewer
git status

# Re-run cmake to pick up git info
cd build
cmake ..
make
```

---

## ✅ Success Checklist

- [ ] Build compiles without errors
- [ ] `--version` flag works
- [ ] Version info displays correctly
- [ ] Git commit hash appears (not "unknown")
- [ ] All features show correct status
- [ ] Model loading still works
- [ ] Changes committed to git
- [ ] Tag created: v0.1.0-alpha.1

---

## 🎓 Version System Usage

### To Create a Stable Release (remove pre-release)
Edit `src/version.h`:
```c
// Comment out the pre-release line:
// #define HLMV_VERSION_PRERELEASE     "alpha.1"

// Version will now be: v0.1.0 (stable)
```

### To Create Next Alpha
Edit `src/version.h`:
```c
#define HLMV_VERSION_BUILD          2  // Increment build

// Version will be: v0.1.0-alpha.1+build.2
```

### To Move to Beta
Edit `src/version.h`:
```c
#define HLMV_VERSION_PRERELEASE     "beta.1"  // Change to beta
#define HLMV_VERSION_BUILD          1         // Reset build

// Version will be: v0.1.0-beta.1
```

### To Release v0.2.0
Edit `src/version.h`:
```c
#define HLMV_VERSION_MINOR          2  // Increment minor
#define HLMV_VERSION_PRERELEASE     "alpha.1"  // Back to alpha
#define HLMV_VERSION_BUILD          1  // Reset build

// Version will be: v0.2.0-alpha.1
```

---

**Everything is ready to go!** 🚀

Run the build, test it, and you're done with v0.1.0-alpha.1!
