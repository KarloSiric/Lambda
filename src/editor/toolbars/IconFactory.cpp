/*
 * ===============================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===============================================================================
 *
 *   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *   Author: Karlo Siric
 *   Purpose: Programmatic icon generation - 32x32 flat icons, consistent style
 * ===============================================================================
 */

#include "IconFactory.h"
#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <cmath>

static const int  SZ  = 32;         // icon size
static const int  M   = 3;          // margin
static QColor     INK( 30, 30, 30 ); // near-black foreground

static QPixmap makePixmap() {
	QPixmap px( SZ, SZ );
	px.fill( Qt::transparent );
	return px;
}

// ─── helpers ────────────────────────────────────────────────────────────────

// Draw an isometric cube outline given a center and half-size
static void drawIsoCube( QPainter &p, float cx, float cy, float hs, bool filled ) {
	// 6 vertices of an isometric cube
	//   top:    cx, cy - hs
	//   right:  cx + hs*cos30, cy - hs*sin30  → approx (+hs*.87, -hs*.5)
	//   left:   cx - hs*cos30, cy - hs*sin30
	//   bottom: cx, cy + hs
	//   botR:   cx + hs*cos30, cy + hs*sin30
	//   botL:   cx - hs*cos30, cy + hs*sin30
	float dx = hs * 0.866f;
	float dy = hs * 0.5f;

	QPointF top(  cx,      cy - hs );
	QPointF r(    cx + dx, cy - dy );
	QPointF l(    cx - dx, cy - dy );
	QPointF bot(  cx,      cy + hs );
	QPointF br(   cx + dx, cy + dy );
	QPointF bl(   cx - dx, cy + dy );

	if ( filled ) {
		// top face
		QPolygonF top_face;
		top_face << top << r << QPointF( cx, cy ) << l;
		p.setBrush( QColor( 80, 80, 80 ) );
		p.drawPolygon( top_face );

		// right face
		QPolygonF right_face;
		right_face << QPointF( cx, cy ) << r << br << bot;
		p.setBrush( QColor( 50, 50, 50 ) );
		p.drawPolygon( right_face );

		// left face
		QPolygonF left_face;
		left_face << l << QPointF( cx, cy ) << bot << bl;
		p.setBrush( QColor( 120, 120, 120 ) );
		p.drawPolygon( left_face );
	}

	// Edges
	p.setBrush( Qt::NoBrush );
	// top face outline
	p.drawLine( top, r );
	p.drawLine( top, l );
	p.drawLine( r,   QPointF( cx, cy ) );
	p.drawLine( l,   QPointF( cx, cy ) );
	// bottom
	p.drawLine( QPointF( cx, cy ), bot );
	p.drawLine( r,  br );
	p.drawLine( l,  bl );
	p.drawLine( br, bot );
	p.drawLine( bl, bot );
}

// ─── icons ───────────────────────────────────────────────────────────────────

QIcon IconFactory::wireframe() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing );
	p.setPen( QPen( INK, 1.8f ) );
	drawIsoCube( p, SZ / 2.0f, SZ / 2.0f, 11.0f, false );
	return QIcon( px );
}

QIcon IconFactory::textured() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing );
	p.setPen( QPen( INK, 1.5f ) );
	drawIsoCube( p, SZ / 2.0f, SZ / 2.0f, 11.0f, true );
	return QIcon( px );
}

QIcon IconFactory::bones() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing );
	QPen pen( INK, 2.0f );
	p.setPen( pen );

	// Simple skeleton: vertical spine + shoulders + hips + limbs
	float cx = SZ / 2.0f;

	// Head
	p.setBrush( Qt::NoBrush );
	p.drawEllipse( QPointF( cx, 5 ), 3.5f, 3.5f );

	// Spine
	p.drawLine( QPointF( cx, 9 ), QPointF( cx, 20 ) );

	// Shoulders
	p.drawLine( QPointF( cx - 7, 11 ), QPointF( cx + 7, 11 ) );

	// Arms
	p.drawLine( QPointF( cx - 7, 11 ), QPointF( cx - 10, 20 ) );
	p.drawLine( QPointF( cx + 7, 11 ), QPointF( cx + 10, 20 ) );

	// Hips
	p.drawLine( QPointF( cx - 5, 20 ), QPointF( cx + 5, 20 ) );

	// Legs
	p.drawLine( QPointF( cx - 4, 20 ), QPointF( cx - 5, 29 ) );
	p.drawLine( QPointF( cx + 4, 20 ), QPointF( cx + 5, 29 ) );

	// Joint dots
	p.setBrush( INK );
	for ( auto pt : { QPointF( cx, 9 ), QPointF( cx - 7, 11 ), QPointF( cx + 7, 11 ),
					   QPointF( cx, 20 ), QPointF( cx - 4, 20 ), QPointF( cx + 4, 20 ) } ) {
		p.drawEllipse( pt, 1.5f, 1.5f );
	}

	return QIcon( px );
}

QIcon IconFactory::hitboxes() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing );

	QPen pen( INK, 1.8f );
	pen.setStyle( Qt::DashLine );
	p.setPen( pen );
	p.setBrush( Qt::NoBrush );

	// Outer dashed box
	p.drawRect( M, M, SZ - M * 2 - 1, SZ - M * 2 - 1 );

	// Inner smaller dashed box offset slightly
	pen.setStyle( Qt::DotLine );
	pen.setColor( QColor( 80, 80, 80 ) );
	p.setPen( pen );
	p.drawRect( M + 5, M + 5, SZ - ( M + 5 ) * 2 - 1, SZ - ( M + 5 ) * 2 - 1 );

	return QIcon( px );
}

QIcon IconFactory::attachments() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing );
	p.setPen( QPen( INK, 2.0f ) );

	float cx = SZ / 2.0f, cy = SZ / 2.0f;

	// Diamond shape
	QPolygonF diamond;
	diamond << QPointF( cx, cy - 10 )
			<< QPointF( cx + 7, cy )
			<< QPointF( cx, cy + 10 )
			<< QPointF( cx - 7, cy );
	p.setBrush( QColor( 60, 60, 60 ) );
	p.drawPolygon( diamond );

	// Cross hair lines
	p.setBrush( Qt::NoBrush );
	p.setPen( QPen( INK, 1.5f ) );
	p.drawLine( QPointF( cx, M ), QPointF( cx, cy - 11 ) );
	p.drawLine( QPointF( cx, cy + 11 ), QPointF( cx, SZ - M ) );
	p.drawLine( QPointF( M, cy ), QPointF( cx - 8, cy ) );
	p.drawLine( QPointF( cx + 8, cy ), QPointF( SZ - M, cy ) );

	return QIcon( px );
}

QIcon IconFactory::grid() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing, false );

	QPen thin( INK, 1.0f );
	p.setPen( thin );

	int step = ( SZ - M * 2 ) / 4;
	for ( int i = 0; i <= 4; i++ ) {
		int pos = M + i * step;
		p.drawLine( pos, M, pos, SZ - M );
		p.drawLine( M, pos, SZ - M, pos );
	}

	return QIcon( px );
}

QIcon IconFactory::axes() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing );

	float ox = 8, oy = SZ - 8;  // origin point
	float len = 17.0f;

	// X axis - red
	p.setPen( QPen( QColor( 200, 40, 40 ), 2.0f ) );
	p.drawLine( QPointF( ox, oy ), QPointF( ox + len, oy ) );
	// arrowhead X
	p.drawLine( QPointF( ox + len, oy ), QPointF( ox + len - 4, oy - 3 ) );
	p.drawLine( QPointF( ox + len, oy ), QPointF( ox + len - 4, oy + 3 ) );
	p.setPen( QPen( QColor( 200, 40, 40 ), 1.2f ) );
	p.setFont( QFont( "Arial", 6, QFont::Bold ) );
	p.drawText( QPointF( ox + len + 1, oy + 4 ), "X" );

	// Y axis - green
	p.setPen( QPen( QColor( 40, 160, 40 ), 2.0f ) );
	p.drawLine( QPointF( ox, oy ), QPointF( ox, oy - len ) );
	p.drawLine( QPointF( ox, oy - len ), QPointF( ox - 3, oy - len + 4 ) );
	p.drawLine( QPointF( ox, oy - len ), QPointF( ox + 3, oy - len + 4 ) );
	p.setPen( QPen( QColor( 40, 160, 40 ), 1.2f ) );
	p.drawText( QPointF( ox - 3, oy - len - 1 ), "Y" );

	// Z axis - blue (diagonal, isometric-ish)
	p.setPen( QPen( QColor( 40, 80, 200 ), 2.0f ) );
	float zex = ox + len * 0.6f, zey = oy - len * 0.45f;
	p.drawLine( QPointF( ox, oy ), QPointF( zex, zey ) );
	p.drawLine( QPointF( zex, zey ), QPointF( zex - 4, zey ) );
	p.drawLine( QPointF( zex, zey ), QPointF( zex, zey + 4 ) );
	p.setPen( QPen( QColor( 40, 80, 200 ), 1.2f ) );
	p.drawText( QPointF( zex + 1, zey - 1 ), "Z" );

	return QIcon( px );
}

QIcon IconFactory::ground() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing );

	// Perspective ground plane: horizontal lines converging to center top
	float cx = SZ / 2.0f;
	float horizon = SZ * 0.35f;
	float bottom = SZ - M;

	// Converging vertical lines
	QPen linePen( INK, 1.2f );
	p.setPen( linePen );
	for ( int i = 0; i <= 6; i++ ) {
		float bx = M + i * ( ( SZ - M * 2 ) / 6.0f );
		p.drawLine( QPointF( cx, horizon ), QPointF( bx, bottom ) );
	}

	// Horizontal lines (3 rows)
	for ( int row = 1; row <= 3; row++ ) {
		float t = row / 4.0f;
		float y = horizon + t * ( bottom - horizon );
		float xLeft  = cx - t * ( cx - M );
		float xRight = cx + t * ( cx - M );
		p.drawLine( QPointF( xLeft, y ), QPointF( xRight, y ) );
	}

	// Horizon line (slightly thicker)
	p.setPen( QPen( INK, 1.8f ) );
	p.drawLine( QPointF( M, horizon ), QPointF( SZ - M, horizon ) );

	return QIcon( px );
}

QIcon IconFactory::cameraReset() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing );
	p.setPen( QPen( INK, 1.8f ) );
	p.setBrush( QColor( 60, 60, 60 ) );

	// Camera body
	p.drawRoundedRect( 5, 11, 18, 12, 2, 2 );

	// Lens circle
	p.setBrush( QColor( 180, 180, 180 ) );
	p.drawEllipse( QPointF( 14, 17 ), 4.5f, 4.5f );
	p.setBrush( QColor( 100, 100, 100 ) );
	p.drawEllipse( QPointF( 14, 17 ), 2.5f, 2.5f );

	// Viewfinder bump
	p.setBrush( QColor( 60, 60, 60 ) );
	p.drawRect( 9, 7, 5, 4 );

	// Reset arrow (arc + arrowhead, top-right)
	p.setBrush( Qt::NoBrush );
	p.setPen( QPen( QColor( 0, 120, 200 ), 2.0f ) );
	p.drawArc( QRect( 19, 4, 10, 10 ), 45 * 16, 270 * 16 );
	// arrowhead
	p.drawLine( 25, 4, 29, 5 );
	p.drawLine( 29, 5, 28, 9 );

	return QIcon( px );
}

QIcon IconFactory::viewFront() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing );
	p.setPen( QPen( INK, 1.5f ) );
	p.setBrush( QColor( 235, 235, 235 ) );
	p.drawRect( M, M, SZ - M * 2 - 1, SZ - M * 2 - 1 );

	// Center cross
	p.setPen( QPen( QColor( 0, 120, 200 ), 1.2f ) );
	float cx = SZ / 2.0f, cy = SZ / 2.0f;
	p.drawLine( QPointF( cx, M + 2 ), QPointF( cx, SZ - M - 2 ) );
	p.drawLine( QPointF( M + 2, cy ), QPointF( SZ - M - 2, cy ) );

	// Label
	p.setPen( INK );
	p.setFont( QFont( "Arial", 7, QFont::Bold ) );
	p.drawText( QRect( 0, 0, SZ, SZ ), Qt::AlignCenter, "F" );

	return QIcon( px );
}

QIcon IconFactory::viewTop() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing );
	p.setPen( QPen( INK, 1.5f ) );
	p.setBrush( QColor( 235, 235, 235 ) );
	p.drawRect( M, M, SZ - M * 2 - 1, SZ - M * 2 - 1 );

	p.setPen( QPen( QColor( 40, 160, 40 ), 1.2f ) );
	float cx = SZ / 2.0f, cy = SZ / 2.0f;
	p.drawLine( QPointF( cx, M + 2 ), QPointF( cx, SZ - M - 2 ) );
	p.drawLine( QPointF( M + 2, cy ), QPointF( SZ - M - 2, cy ) );

	p.setPen( INK );
	p.setFont( QFont( "Arial", 7, QFont::Bold ) );
	p.drawText( QRect( 0, 0, SZ, SZ ), Qt::AlignCenter, "T" );

	return QIcon( px );
}

QIcon IconFactory::viewSide() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing );
	p.setPen( QPen( INK, 1.5f ) );
	p.setBrush( QColor( 235, 235, 235 ) );
	p.drawRect( M, M, SZ - M * 2 - 1, SZ - M * 2 - 1 );

	p.setPen( QPen( QColor( 200, 40, 40 ), 1.2f ) );
	float cx = SZ / 2.0f, cy = SZ / 2.0f;
	p.drawLine( QPointF( cx, M + 2 ), QPointF( cx, SZ - M - 2 ) );
	p.drawLine( QPointF( M + 2, cy ), QPointF( SZ - M - 2, cy ) );

	p.setPen( INK );
	p.setFont( QFont( "Arial", 7, QFont::Bold ) );
	p.drawText( QRect( 0, 0, SZ, SZ ), Qt::AlignCenter, "S" );

	return QIcon( px );
}

QIcon IconFactory::gridIncrease() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing, false );

	// Sparse grid (large cell = large grid spacing)
	p.setPen( QPen( INK, 1.2f ) );
	int step = ( SZ - M * 2 ) / 2;
	for ( int i = 0; i <= 2; i++ ) {
		int pos = M + i * step;
		p.drawLine( pos, M, pos, SZ - M );
		p.drawLine( M, pos, SZ - M, pos );
	}

	// + symbol (bottom-right)
	p.setPen( QPen( QColor( 0, 140, 0 ), 2.5f ) );
	p.drawLine( 22, 27, 29, 27 );
	p.drawLine( 25, 24, 25, 30 );

	return QIcon( px );
}

QIcon IconFactory::gridDecrease() {
	QPixmap px = makePixmap();
	QPainter p( &px );
	p.setRenderHint( QPainter::Antialiasing, false );

	// Dense grid (small cell = small grid spacing)
	p.setPen( QPen( INK, 1.0f ) );
	int step = ( SZ - M * 2 ) / 4;
	for ( int i = 0; i <= 4; i++ ) {
		int pos = M + i * step;
		p.drawLine( pos, M, pos, SZ - M - 10 );
		p.drawLine( M, pos, SZ - M - 10, pos );
	}

	// − symbol (bottom-right)
	p.setPen( QPen( QColor( 200, 60, 0 ), 2.5f ) );
	p.drawLine( 22, 27, 29, 27 );

	return QIcon( px );
}
