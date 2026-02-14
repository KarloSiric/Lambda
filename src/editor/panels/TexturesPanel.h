/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   TexturesPanel.h  -  Texture thumbnail browser (BSP/Quake editor style)
 *
 * ===========================================================================
 */

#ifndef TEXTURES_PANEL_H
#define TEXTURES_PANEL_H

#include "InspectorPanel.h"
#include <QComboBox>
#include <QLabel>
#include <QScrollArea>
#include <QVector>
#include <QImage>
#include "../widgets/FlowLayout.h"

class QContextMenuEvent;

// Simple texture thumbnail widget - just image + name below
class TextureThumbnail : public QWidget {
	Q_OBJECT

  public:
	explicit TextureThumbnail( int index, const QImage &image, const QString &name, QWidget *parent = nullptr );

	int textureIndex() const { return m_index; }
	void setSelected( bool selected );
	bool isSelected() const { return m_selected; }

  signals:
	void clicked( int index );
	void doubleClicked( int index );
	void rightClicked( int index, const QPoint &globalPos );

  protected:
	void paintEvent( QPaintEvent *event ) override;
	void mousePressEvent( QMouseEvent *event ) override;
	void mouseDoubleClickEvent( QMouseEvent *event ) override;
	void enterEvent( QEnterEvent *event ) override;
	void leaveEvent( QEvent *event ) override;

  private:
	int m_index;
	QPixmap m_pixmap;
	QString m_name;
	bool m_selected;
	bool m_hovered;
};

class TexturesPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit TexturesPanel( QWidget *parent = nullptr );

	void refresh() override;

  signals:
	void requestTextureTab( const QImage &image, const QString &name, int width, int height, int flags );

  private slots:
	void onSkinFamilyChanged( int index );
	void onThumbnailClicked( int index );
	void onThumbnailDoubleClicked( int index );
	void onThumbnailRightClicked( int index, const QPoint &globalPos );

  private:
	void setupUI();
	void populateThumbnails();
	void updateSelectionInfo( int textureIndex );
	void clearThumbnails();
	void showTexturePreview( int textureIndex );
	void selectThumbnail( int index );
	QString formatTextureFlags( int flags ) const;

	QComboBox *m_skinSelector;
	QScrollArea *m_scrollArea;
	QWidget *m_thumbnailContainer;
	FlowLayout *m_flowLayout;
	QLabel *m_selectionInfo;
	int m_selectedTextureIndex;

	QVector<TextureThumbnail *> m_thumbnails;
	QVector<QImage> m_cachedFullImages;
};

#endif // TEXTURES_PANEL_H
