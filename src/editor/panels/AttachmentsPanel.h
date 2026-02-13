/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   AttachmentsPanel.h  -  Attachment points viewer panel
 *
 *   Displays model attachment points with:
 *   - Table listing all attachments
 *   - Details for selected attachment (bone, position)
 *
 * ===========================================================================
 */

#ifndef ATTACHMENTS_PANEL_H
#define ATTACHMENTS_PANEL_H

#include "InspectorPanel.h"
#include <QTableWidget>
#include <QLabel>

class AttachmentsPanel : public InspectorPanel {
	Q_OBJECT

  public:
	explicit AttachmentsPanel( QWidget *parent = nullptr );

	void refresh() override;

  private slots:
	void onAttachmentSelected( int row, int column );

  private:
	void setupUI();
	void populateAttachments();
	void updateInfoBox( int attachmentIndex );

	QTableWidget *m_attachmentTable;
	QLabel *m_infoLabel;
};

#endif // ATTACHMENTS_PANEL_H
