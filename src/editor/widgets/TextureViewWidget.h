/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   TextureViewWidget.h  -  Texture viewer widget for tab display
 *
 *   Simple widget for viewing textures in the main viewport tab area.
 *   Features:
 *   - Full-size texture display with scroll
 *   - Info bar showing dimensions and flags
 *   - Black background for proper texture viewing
 *
 * ===========================================================================
 */

#ifndef TEXTURE_VIEW_WIDGET_H
#define TEXTURE_VIEW_WIDGET_H

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QScrollArea>

class TextureViewWidget : public QWidget {
	Q_OBJECT

  public:
	explicit TextureViewWidget( const QImage &image, const QString &name,
	                            int width, int height, int flags,
	                            QWidget *parent = nullptr );

	QString textureName() const { return m_name; }

  private:
	void setupUI();
	QString formatTextureFlags( int flags ) const;

	QImage m_image;
	QString m_name;
	int m_width;
	int m_height;
	int m_flags;

	QScrollArea *m_scrollArea;
	QLabel *m_imageLabel;
	QLabel *m_infoLabel;
};

#endif // TEXTURE_VIEW_WIDGET_H
