/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   ModelInfoPanel.h  -  Model metadata property grid
 *
 *   Displays read-only model information in a two-column property grid:
 *   - File info (name, path, size, version)
 *   - Geometry counts (vertices, triangles, bodyparts)
 *   - Structure counts (bones, sequences, textures)
 *   - Bounding box and eye position
 *
 * ===========================================================================
 */

#ifndef MODEL_INFO_PANEL_H
#define MODEL_INFO_PANEL_H

#include "InspectorPanel.h"
#include <QTableWidget>
#include <QPushButton>

class ModelInfoPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit ModelInfoPanel( QWidget *parent = nullptr );

	void refresh() override;

  private slots:
	void onDumpModelInfo();
	void onDumpTextureInfo();

  private:
	void setupUI();
	void addProperty( const QString &name, const QString &value, bool isSeparator = false );
	void clearProperties();
	QString formatFileSize( qint64 bytes ) const;
	QString formatVector( float x, float y, float z ) const;
	QString generateModelReport() const;
	QString generateTextureReport() const;
	void showDumpDialog( const QString &title, const QString &content );

	QTableWidget *m_propertyTable;
	QPushButton *m_dumpModelBtn;
	QPushButton *m_dumpTextureBtn;
	int m_currentRow;
};

#endif // MODEL_INFO_PANEL_H
