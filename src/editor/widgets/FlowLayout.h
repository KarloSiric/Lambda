/*
 * ===========================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===========================================================================
 *
 *   FlowLayout.h  -  Flowing grid layout (wraps items to next row)
 *
 * ===========================================================================
 */

#ifndef FLOW_LAYOUT_H
#define FLOW_LAYOUT_H

#include <QLayout>
#include <QStyle>

class FlowLayout : public QLayout {
  public:
	explicit FlowLayout( QWidget *parent = nullptr, int margin = -1, int hSpacing = -1, int vSpacing = -1 );
	~FlowLayout() override;

	void addItem( QLayoutItem *item ) override;
	int horizontalSpacing() const;
	int verticalSpacing() const;
	Qt::Orientations expandingDirections() const override;
	bool hasHeightForWidth() const override;
	int heightForWidth( int width ) const override;
	int count() const override;
	QLayoutItem *itemAt( int index ) const override;
	QSize minimumSize() const override;
	void setGeometry( const QRect &rect ) override;
	QSize sizeHint() const override;
	QLayoutItem *takeAt( int index ) override;

  private:
	int doLayout( const QRect &rect, bool testOnly ) const;
	int smartSpacing( QStyle::PixelMetric pm ) const;

	QList<QLayoutItem *> m_itemList;
	int m_hSpace;
	int m_vSpace;
};

#endif // FLOW_LAYOUT_H
