#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "ModelViewport.h"
#include <QMainWindow>
#include <QDockWidget>
#include <QWidget>
#include <QOpenGLWidget>
#include <QTabWidget>
#include <QtWidgets/qmainwindow.h>


#define MW_WIDTH 1920
#define MW_HEIGHT 1080


#define MW_CONSOLE_MIN_HEIGHT 300
#define MW_CONSOLE_MAX_HEIGHT 1000


#define MW_RIGHT_DOCK_MIN_WIDTH 400
#define MW_RIGHT_DOCK_MAX_WIDTH 1000


class MainWindow : public QMainWindow { 
    
	Q_OBJECT
    
  public:
    
	explicit MainWindow( QWidget *parent = nullptr );
    
  private slots:

    void onOpenModel();
    void onNewTab();
    void onCloseTab(int index);
    void onTabChanged(int index);

  private:
     
	void setupMenus();
	void setupViewports();
	void setupDocks();
    void setupToolbars();
    void setupTheme();
    
    
	void createFileMenu();
	void createEditMenu();
	void createViewMenu();
    void createModelMenu();
    void createBodypartMenu();
    void createBonesMenu();
    void createSequencesMenu();
    void createTexturesMenu();
	void createToolsMenu();
	void createDebugMenu();
	void createWindowMenu();
	void createHelpMenu();
	void createToolbarUpper();
    void createToolbarLower();


	void createDocks();
	void createViewportContainer();

	// Helper functions for tab management
	ModelViewport* createNewViewport();
	ModelViewport* getCurrentViewport();
	int addViewportTab(const QString &title = "New Model");

	QDockWidget *leftDock;
	QDockWidget *rightDock;
	QDockWidget *bottomDock;
	QStatusBar *statusBar;

	// Central tab widget (replaces single viewport)
	QTabWidget *tabWidget;
};

#endif
