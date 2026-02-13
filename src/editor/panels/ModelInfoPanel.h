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

class ModelInfoPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit ModelInfoPanel( QWidget *parent = nullptr );

	void refresh() override;

  private:
	void setupUI();
	void addProperty( const QString &name, const QString &value, bool isSeparator = false );
	void clearProperties();
	QString formatFileSize( qint64 bytes ) const;
	QString formatVector( float x, float y, float z ) const;

	QTableWidget *m_propertyTable;
	int m_currentRow;
};

#endif // MODEL_INFO_PANEL_H
