#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QWidget>
#include <QOpenGLWidget>
#include <QtWidgets/qmainwindow.h>

#define MW_WIDTH                    1920
#define MW_HEIGHT                   1080

#define MW_CONSOLE_MIN_HEIGHT       350
#define MW_CONSOLE_MAX_HEIGHT       1000

#define MW_RIGHT_DOCK_MIN_WIDTH     350 
#define MW_RIGHT_DOCK_MAX_WIDTH     1000 


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
    QStatusBar *statusBar;

    QWidget *viewportContainer;

    void createDocks();
    void createViewportContainer();
};

#endif
