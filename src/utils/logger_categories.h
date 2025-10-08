#ifndef LOGCATEGORIES_H
#define LOGCATEGORIES_H

#pragma once

// Short, readable, maps 1:1 to subsystems
#define LOG_CAT_APP      "app"         // app bootstrap / CLI / lifecycle
#define LOG_CAT_IO       "io"          // file open/read/write/paths
#define LOG_CAT_MDL      "mdl"         // MDL parsing, headers, bodyparts/models/meshes
#define LOG_CAT_TEXTURES "textures"    // palettes, flags, GL texture upload
#define LOG_CAT_RENDERER "renderer"    // GL init, GLFW events, frames, modes, shaders
#define LOG_CAT_ANIM     "anim"        // sequences, fps, blending, controllers
#define LOG_CAT_BONES    "bones"       // skeleton setup, transforms
#define LOG_CAT_BODYPART "bodypart"    // selection/variants/skin families
#define LOG_CAT_SHADERS  "shaders"     // shader loading, defines, uniforms
#define LOG_CAT_UI       "ui"          // future: Qt/SDL UI



#endif
