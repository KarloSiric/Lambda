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
#include <QSlider>
#include <QPushButton>

class TextureViewWidget : public QWidget {
	Q_OBJECT

  public:
	explicit TextureViewWidget( const QImage &image, const QString &name,
	                            int width, int height, int flags,
	                            QWidget *parent = nullptr );

	QString textureName() const { return m_name; }

  protected:
	void wheelEvent( QWheelEvent *event ) override;

  private slots:
	void onZoomIn();
	void onZoomOut();
	void onZoomFit();
	void onZoom100();
	void onZoomSliderChanged( int value );

  private:
	void setupUI();
	void updateImageDisplay();
	QString formatTextureFlags( int flags ) const;
	void updateZoomLabel();

	QImage m_image;
	QString m_name;
	int m_width;
	int m_height;
	int m_flags;
	float m_zoomLevel;  // 1.0 = 100%, 2.0 = 200%, etc.

	QScrollArea *m_scrollArea;
	QLabel *m_imageLabel;
	QLabel *m_infoLabel;
	QLabel *m_zoomLabel;
	QSlider *m_zoomSlider;
};

#endif // TEXTURE_VIEW_WIDGET_H
