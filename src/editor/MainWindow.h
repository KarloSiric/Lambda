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
#include <QComboBox>
#include <QMap>
#include <QTimer>
#include <QElapsedTimer>
#include <QAction>
#include <QtCore/qtmetamacros.h>
#include <QtWidgets/qtextedit.h>

// Forward declarations for inspector panels
class BrowserPanel;
class ModelInfoPanel;
class SequencesPanel;
class TexturesPanel;
class BodypartsPanel;
class BonesPanel;
class BoneControllersPanel;
class ModelDisplayPanel;
class AttachmentsPanel;
class LightingPanel;


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
    ~MainWindow() override;

  public slots:

    void onOpenModel();
    void onNewTab();
    void onCloseTab( int index );
    void onTabChanged( int index );
    void onStatusBarUpdate();

    // Animation control slots
    void onPlayAnimation();
    void onPauseAnimation();
    void onStopAnimation();
    void onPrevFrame();
    void onNextFrame();
    void onToggleLoop( bool checked );
    void onSequenceChanged( int index );

    // View toggle slots
    void onToggleWireframe( bool checked );
    void onToggleTextured( bool checked );
    void onToggleBones( bool checked );
    void onToggleHitboxes( bool checked );
    void onToggleAttachments( bool checked );
    void onToggleNormals( bool checked );
    void onToggleGrid( bool checked );
    void onToggleAxes( bool checked );
    void onToggleGround( bool checked );
    void onToggleLighting( bool checked );

    // Camera control slots
    void onResetCamera();
    void onCenterModel();

    // Utility slots
    void onScreenshot();
    void onBackgroundColor();

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

	// Inspector panel tabs (three rows)
	QWidget *m_inspectorContainer;
	QTabWidget *m_inspectorTabsRow1;  // Model Info, Display, Animation, Textures
	QTabWidget *m_inspectorTabsRow2;  // Bodyparts, Bones, Attachments, Lighting
	QTabWidget *m_inspectorTabsRow3;  // Files browser
	BrowserPanel *m_browserPanel;
	ModelInfoPanel *m_modelInfoPanel;
	SequencesPanel *m_sequencesPanel;
	TexturesPanel *m_texturesPanel;
	BodypartsPanel *m_bodypartsPanel;
	BonesPanel *m_bonesPanel;
	BoneControllersPanel *m_boneControllersPanel;
	ModelDisplayPanel *m_modelDisplayPanel;
	AttachmentsPanel *m_attachmentsPanel;
	LightingPanel *m_lightingPanel;

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
	QPoint m_lastMousePos;  // Cache mouse position to avoid expensive raycast when unchanged

	// Animation state
	bool m_loopAnimation;

	// Sequence selector combobox (in toolbar)
	QComboBox *m_sequenceSelector;

	// Toolbar toggle actions (stored for syncing with inspector panels)
	QAction *m_actionWireframe;
	QAction *m_actionTextured;
	QAction *m_actionBones;
	QAction *m_actionHitboxes;
	QAction *m_actionAttachments;
	QAction *m_actionNormals;
	QAction *m_actionGrid;
	QAction *m_actionAxes;
	QAction *m_actionGround;
	QAction *m_actionLighting;

	// Helper to connect toolbar actions
	void connectToolbarActions();

	// Sync toolbar toggle states with viewport
	void syncToolbarWithViewport( ModelViewport *viewport );

	// Helper to update sequence list when model changes
	void updateSequenceList( ModelViewport *viewport );

	// Update inspector panels when viewport changes
	void updateInspector( ModelViewport *viewport );

	// Slot for browser panel model selection
	void onBrowserModelSelected( const QString &path );

	// Slot for texture panel's request to open texture in new tab
	void onTextureTabRequested( const QImage &image, const QString &name, int width, int height, int flags );

};

#endif
