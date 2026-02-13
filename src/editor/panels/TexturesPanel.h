/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   TexturesPanel.h  -  Texture thumbnail browser
 *
 *   Displays model textures as clickable thumbnails with:
 *   - Skin family selector dropdown
 *   - Grid of 64x64 texture thumbnails
 *   - Selection info showing texture details
 *
 * ===========================================================================
 */

#ifndef TEXTURES_PANEL_H
#define TEXTURES_PANEL_H

#include "InspectorPanel.h"
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QVector>
#include <QImage>

class QContextMenuEvent;

class TextureThumbnail : public QPushButton {
	Q_OBJECT

  public:
	explicit TextureThumbnail( int index, const QImage &image, const QString &name, QWidget *parent = nullptr );

	int textureIndex() const { return m_index; }

  signals:
	void doubleClicked( int index );
	void openInNewTab( int index );
	void openInSeparateWindow( int index );

  protected:
	void mouseDoubleClickEvent( QMouseEvent *event ) override;
	void contextMenuEvent( QContextMenuEvent *event ) override;

  private:
	int m_index;
};

class TexturesPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit TexturesPanel( QWidget *parent = nullptr );

	void refresh() override;

  signals:
	// Signal to MainWindow to open texture in a new viewport tab
	void requestTextureTab( const QImage &image, const QString &name, int width, int height, int flags );

  private slots:
	void onSkinFamilyChanged( int index );
	void onThumbnailClicked();
	void onThumbnailDoubleClicked( int index );
	void onOpenInNewTab( int index );
	void onOpenInSeparateWindow( int index );

  private:
	void setupUI();
	void populateThumbnails();
	void updateSelectionInfo( int textureIndex );
	void clearThumbnails();
	void showTexturePreview( int textureIndex );
	QString formatTextureFlags( int flags ) const;

	QComboBox *m_skinSelector;
	QWidget *m_thumbnailContainer;
	QGridLayout *m_thumbnailLayout;
	QLabel *m_selectionInfo;
	int m_selectedTextureIndex;

	// Cached texture images for performance
	QVector<QImage> m_cachedFullImages;
};

#endif // TEXTURES_PANEL_H
