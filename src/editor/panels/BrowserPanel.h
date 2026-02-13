/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   BrowserPanel.h  -  File/folder browser panel
 *
 *   Allows browsing folders or .pak archives for models and textures.
 *   Features:
 *   - Open Folder button to browse filesystem
 *   - Open PAK button to browse PAK archives (future)
 *   - Tree view showing files with icons
 *   - Double-click MDL to load in viewport
 *   - Search filter box
 *
 * ===========================================================================
 */

#ifndef BROWSER_PANEL_H
#define BROWSER_PANEL_H

#include "InspectorPanel.h"
#include <QTreeWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QFileSystemModel>

class BrowserPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit BrowserPanel( QWidget *parent = nullptr );

	// Browser doesn't need a viewport, it works independently
	void setViewport( ModelViewport *viewport ) override;
	void refresh() override;

  signals:
	// Emitted when user double-clicks an MDL file
	void modelFileSelected( const QString &path );

  private slots:
	void onOpenFolder();
	void onOpenPak();
	void onSearchTextChanged( const QString &text );
	void onItemDoubleClicked( QTreeWidgetItem *item, int column );
	void onItemExpanded( QTreeWidgetItem *item );

  private:
	void setupUI();
	void populateFolder( const QString &folderPath );
	void populateTreeItem( QTreeWidgetItem *parentItem, const QString &dirPath );
	void filterTree( const QString &filter );
	bool itemMatchesFilter( QTreeWidgetItem *item, const QString &filter );
	QIcon getFileIcon( const QString &fileName ) const;

	QPushButton *m_openFolderBtn;
	QPushButton *m_openPakBtn;
	QLineEdit *m_searchBox;
	QTreeWidget *m_fileTree;

	QString m_currentFolder;
	QString m_currentFilter;
};

#endif // BROWSER_PANEL_H
