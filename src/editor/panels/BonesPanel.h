/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   BonesPanel.h  -  Bone hierarchy tree viewer
 *
 *   Displays the model's skeleton as a hierarchical tree where each bone
 *   shows its parent-child relationships. Read-only view for inspecting
 *   bone structure.
 *
 * ===========================================================================
 */

#ifndef BONES_PANEL_H
#define BONES_PANEL_H

#include "InspectorPanel.h"
#include <QTreeWidget>

class BonesPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit BonesPanel( QWidget *parent = nullptr );

	void refresh() override;

  private slots:
	void onBoneSelected( QTreeWidgetItem *item, int column );

  private:
	void setupUI();
	void buildBoneTree();

	QTreeWidget *m_boneTree;
	QLabel *m_infoLabel;
};

#endif // BONES_PANEL_H
