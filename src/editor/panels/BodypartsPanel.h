/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   BodypartsPanel.h  -  Bodypart/Submodel selector
 *
 *   Allows selection of active submodel for each bodypart. Half-Life models
 *   can have multiple bodyparts (e.g., body, head, weapons) and each can
 *   have multiple submodels (variations).
 *
 * ===========================================================================
 */

#ifndef BODYPARTS_PANEL_H
#define BODYPARTS_PANEL_H

#include "InspectorPanel.h"
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QScrollArea>
#include <QMap>

class BodypartsPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit BodypartsPanel( QWidget *parent = nullptr );

	void refresh() override;

  private slots:
	void onSubmodelSelected( int bodypartIndex, int submodelIndex );

  private:
	void setupUI();
	void clearBodyparts();
	void populateBodyparts();

	QScrollArea *m_scrollArea;
	QWidget *m_bodypartContainer;
	QVBoxLayout *m_bodypartLayout;

	// Map bodypart index to its button group
	QMap<int, QButtonGroup *> m_buttonGroups;
};

#endif // BODYPARTS_PANEL_H
