/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer/Editor ~ Lambda
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
 *   Purpose: Editor configuration and user preferences system
 * ═══════════════════════════════════════════════════════════════════════════
 */

#ifndef EDITORCONFIG_H
#define EDITORCONFIG_H

#include <QString>
#include <QFont>
#include <QColor>
#include <QJsonObject>

/**
 * User-configurable editor preferences
 * Loaded from/saved to config.json in user's home directory
 */
class EditorConfig {
public:
    // UI Configuration
    struct UIConfig {
        struct StatusBarConfig {
            QString fontFamily = "Segoe UI";
            int fontSize = 11;
            bool visible = true;
            bool showFPS = true;
            bool showCPU = true;
            bool showGPU = true;
            bool showRAM = true;
            bool showGrid = true;
            bool showCamera = true;
        } statusBar;

        struct ConsoleConfig {
            QString fontFamily = "Consolas";
            int fontSize = 10;
            int maxLines = 10000;
            bool timestamps = true;
            bool autoScroll = true;
            bool showTrace = false;
            bool showDebug = false;
            bool showInfo = true;
            bool showWarn = true;
            bool showError = true;
            bool showFatal = true;
        } console;

        struct ViewportConfig {
            QColor backgroundColor = QColor(45, 45, 45);
            QColor gridColor = QColor(80, 80, 80);
            float gridSize = 10.0f;
            bool showGrid = true;
            bool showAxis = true;
            int fov = 75;
            float nearClip = 0.1f;
            float farClip = 10000.0f;
        } viewport;

        struct WindowConfig {
            int width = 1920;
            int height = 1080;
            bool maximized = false;
            bool inspectorVisible = true;
            bool consoleVisible = true;
            int consoleSplitRatio = 70;  // Percentage
        } window;
    } ui;

    // Rendering Configuration
    struct RenderConfig {
        bool vsync = true;
        int msaaSamples = 4;  // Anti-aliasing
        bool wireframeMode = false;
        bool showNormals = false;
        bool showBones = false;
        bool showHitboxes = false;
        bool showAttachments = false;
    } render;

    // Animation Configuration
    struct AnimConfig {
        float defaultPlaybackSpeed = 1.0f;
        bool loopAnimations = true;
        bool interpolateFrames = true;
    } anim;

    // Recent Files
    struct RecentFilesConfig {
        QStringList paths;
        int maxRecent = 10;
    } recentFiles;

    // Methods
    static EditorConfig& instance();

    bool load(const QString &configPath = "");
    bool save(const QString &configPath = "");

    QString getDefaultConfigPath() const;

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

private:
    EditorConfig() = default;
    EditorConfig(const EditorConfig&) = delete;
    EditorConfig& operator=(const EditorConfig&) = delete;
};

#endif // EDITORCONFIG_H
