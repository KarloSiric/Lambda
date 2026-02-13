/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   InspectorPanel.h  -  Base class for all Inspector dock panels
 *
 *   Each panel in the Inspector (Model Info, Sequences, Textures, etc.)
 *   inherits from this base class which provides:
 *   - Standard setViewport() interface
 *   - Common styling helpers
 *   - Refresh mechanism when model changes
 *
 * ===========================================================================
 */

#ifndef INSPECTOR_PANEL_H
#define INSPECTOR_PANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>

// Forward declaration
class ModelViewport;

class InspectorPanel : public QWidget {
	Q_OBJECT

  public:
	explicit InspectorPanel( QWidget *parent = nullptr );
	virtual ~InspectorPanel() = default;

	// Set the viewport this panel should display data from
	virtual void setViewport( ModelViewport *viewport );

	// Get the current viewport
	ModelViewport *viewport() const { return m_viewport; }

	// Check if a model is currently loaded
	bool hasModel() const;

  public slots:
	// Called when model is loaded/closed - subclasses override to refresh
	virtual void onModelLoaded();
	virtual void onModelClosed();
	virtual void refresh();

  protected:
	// Create a styled group box (collapsible section within a tab)
	QGroupBox *createSection( const QString &title, bool collapsed = false );

	// Create a "No model loaded" placeholder
	QLabel *createPlaceholder( const QString &message = "No model loaded" );

	// Helper to show/hide placeholder vs content
	void showPlaceholder( bool show );

	ModelViewport *m_viewport;
	QVBoxLayout *m_mainLayout;
	QWidget *m_contentWidget;
	QLabel *m_placeholderLabel;
};

#endif // INSPECTOR_PANEL_H
