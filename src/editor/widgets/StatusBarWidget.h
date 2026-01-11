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
 *   Purpose: Professional status bar widget with model info and panel toggles
 * ═══════════════════════════════════════════════════════════════════════════
 */

#ifndef STATUSBARWIDGET_H
#define STATUSBARWIDGET_H

#include <QStatusBar>
#include <QLabel>
#include <QPushButton>

class StatusBarWidget : public QStatusBar {
    Q_OBJECT

public:
    explicit StatusBarWidget(QWidget *parent = nullptr);
    ~StatusBarWidget();

    void setModelInfo(const QString &filePath, int vertexCount, int triangleCount,
                      int boneCount, int sequenceCount, int textureCount);
    void setFileSize(qint64 sizeBytes);
    void setSequenceInfo(const QString &sequenceName, int currentFrame, int totalFrames);
    void setBoneName(const QString &boneName);
    void setControllerName(const QString &controllerName);
    void setSystemStats(float cpuUsage, float gpuUsage, float ramUsage);
    void setCameraDistance(float distance);
    void setGridSize(float size);
    void setFPS(int fps);
    void setResolution(int width, int height);
    void clearModelInfo();

    void setInspectorVisible(bool visible);
    void setConsoleVisible(bool visible);
    void setMemoryVisible(bool visible);

signals:
    void inspectorToggleRequested();
    void consoleToggleRequested();
    void memoryToggleRequested();

private slots:
    void onInspectorButtonClicked();
    void onConsoleButtonClicked();
    void onMemoryButtonClicked();
    void showInspectorContextMenu(const QPoint &pos);
    void showConsoleContextMenu(const QPoint &pos);
    void showMemoryContextMenu(const QPoint &pos);

private:
    void setupUI();
    void createModelInfoWidgets();
    void createViewInfoWidgets();
    void createToggleButtons();
    void applyStyles();

    QLabel *m_filePathLabel;
    QLabel *m_vertexCountLabel;
    QLabel *m_triangleCountLabel;
    QLabel *m_boneCountLabel;
    QLabel *m_sequenceCountLabel;
    QLabel *m_textureCountLabel;
    QLabel *m_fileSizeLabel;

    QLabel *m_sequenceInfoLabel;
    QLabel *m_boneNameLabel;
    QLabel *m_controllerNameLabel;

    QLabel *m_cpuUsageLabel;
    QLabel *m_gpuUsageLabel;
    QLabel *m_ramUsageLabel;

    QLabel *m_gridSizeLabel;
    QLabel *m_cameraDistLabel;
    QLabel *m_resolutionLabel;
    QLabel *m_fpsLabel;

    QPushButton *m_inspectorToggle;
    QPushButton *m_consoleToggle;
    QPushButton *m_memoryToggle;

    QString m_currentFilePath;
    int m_vertexCount;
    int m_triangleCount;
    int m_boneCount;
    int m_sequenceCount;
    int m_textureCount;
    float m_gridSize;
    float m_cameraDistance;
    int m_viewportWidth;
    int m_viewportHeight;
    int m_currentFPS;
};

#endif // STATUSBARWIDGET_H
