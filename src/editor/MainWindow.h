#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "ModelViewport.h"
#include "StatusBarWidget.h"
#include "ConsoleWidget.h"
#include "LogWidget.h"
#include <QMainWindow>
#include <QDockWidget>
#include <QWidget>
#include <QOpenGLWidget>
#include <QTabWidget>
#include <QMap>
#include <QTimer>
#include <QElapsedTimer>
#include <QtWidgets/qtextedit.h>


#define MW_WIDTH 1920
#define MW_HEIGHT 1080


#define MW_CONSOLE_MIN_HEIGHT 250
#define MW_CONSOLE_MAX_HEIGHT 1000


#define MW_RIGHT_DOCK_MIN_WIDTH 300
#define MW_RIGHT_DOCK_MAX_WIDTH 1200


class MainWindow : public QMainWindow { 
    
	Q_OBJECT
    
  public:

	explicit MainWindow( QWidget *parent = nullptr );

  public slots:

    void onOpenModel();
    void onNewTab();
    void onCloseTab( int index );
    void onTabChanged( int index );
    void onStatusBarUpdate();

  private:
     
	void setupMenus();
	void setupViewports();
	void setupDocks();
    void setupToolbars();
    void setupTheme();

	void createDocks();
	void createViewportContainer();

	// Helper functions for tab management
	ModelViewport* createNewViewport();
	ModelViewport* getCurrentViewport();
	int addViewportTab(const QString &title = "New Model");

	StatusBarWidget *m_statusBar;
    ConsoleWidget *m_consoleWidget;
    LogWidget *m_logWidget;
	QDockWidget *leftDock;
	QDockWidget *rightDock;
	QDockWidget *bottomDock;
	QTabWidget *tabWidget;

	// Store per-tab model info for status bar
	struct TabModelInfo {
		QString filePath;
		qint64 fileSize = 0;
		int vertexCount = 0;
		int triangleCount = 0;
		int boneCount = 0;
		int sequenceCount = 0;
		int textureCount = 0;
		QString currentSequence;
		int currentFrame = 0;
		int totalFrames = 0;
		QString selectedBone;
		QString activeController;
	};
	QMap<int, TabModelInfo> m_tabModelInfo;

	// Real-time status bar update timer
	QTimer *m_statusUpdateTimer;
	QElapsedTimer m_fpsTimer;
	int m_frameCount;
	float m_lastFps;
};

#endif
