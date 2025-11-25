#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QWidget>
#include <QOpenGLWidget>

#define MW_WIDTH 1600
#define MW_HEIGHT 900

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupMenus();
    void setupToolbars();
    void setupLeftToolbox();
    void setupRightInspector();
    void setupViewports();

    void createFileMenu();
    void createEditMenu();
    void createViewMenu();
    void createToolsMenu();
    void createDebugMenu();
    void createWindowMenu();
    void createHelpMenu();

    void setTheme();

    QDockWidget *leftDock;
    QDockWidget *rightDock;
    QDockWidget *bottomDock;

    QWidget *viewportContainer;

    void createDocks();
    void createViewportContainer();
};

#endif
