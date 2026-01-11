#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "ModelViewport.h"
#include <QMainWindow>
#include <QDockWidget>
#include <QWidget>
#include <QMenu>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QOpenGLWidget>
#include <QTabWidget>
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qlabel.h>
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
    void onCloseTab( int index );
    void onTabChanged( int index );
    
    // Adding for the console and inspector control slots
    void onToggleConsole( );
    void onToggleInspector( );
    void updateStatusBar( );
    void updateFPS( );    

  private:
    
    // Adding for the status bar and for the 
    
    QLabel *m_fileNameLabel;
    QLabel *m_fileSizeLabel;
    QLabel *m_vertexCountLabel;
    QLabel *m_polyCountLabel;
    QLabel *m_boneCountLabel;
    QLabel *m_sequenceCountLabel;
    QLabel *m_textureCountLabel;
    
    QLabel *m_cameraDistLabel;
    QLabel *m_fpsCountLabel;
    QLabel *m_gridSizeCountLabel; 
    
    QPushButton *m_memoryToggle;
    QPushButton *m_inspectorToggle;
    QPushButton *m_consoleToggle;
    
    QTimer *m_fpsTimer;
    int m_frameCount;
    double m_lastFPSUpdate;
     
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
