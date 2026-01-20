/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 *   This product contains software technology licensed from Id
 *   Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 *   All Rights Reserved.
 *
 *   Use, distribution, and modification of this source code and/or resulting
 *   object code is restricted to non-commercial enhancements to products from
 *   Valve LLC. All other use, distribution, or modification is prohibited
 *   without written permission from Valve LLC.
 *
 * ───────────────────────────────────────────────────────────────────────────
 *   Author: Karlo Siric
 *   Purpose: Qt Console Widget Implementation for the console panel layout
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "ConsoleWidget.h"

ConsoleWidget::ConsoleWidget( QWidget *parent )
	: QWidget( parent ),
	  m_textEdit( nullptr ),
	  m_queueTimer( nullptr ),
	  m_delayEnabled( true ),
	  m_delayInterval( 50 ) {
	setupUI();
	applyStyles();

	// Setting up the queue timer
	m_queueTimer = new QTimer( this );
	m_queueTimer->setSingleShot( true );
	connect( m_queueTimer, &QTimer::timeout, this, &ConsoleWidget::processQueue );
}

ConsoleWidget::~ConsoleWidget() {
	// DESTRUCTOR
}

void ConsoleWidget::setupUI( void ) {
	QVBoxLayout *v_layout = new QVBoxLayout( this );
	v_layout->setContentsMargins( 0, 0, 0, 0 );
	v_layout->setSpacing( 0 );

	m_textEdit = new QTextEdit();
	m_textEdit->setReadOnly( true );
	m_textEdit->setLineWrapMode( QTextEdit::NoWrap );
	m_textEdit->setAcceptRichText( true );

	v_layout->addWidget( m_textEdit );
}

void ConsoleWidget::applyStyles( void ) {
	m_textEdit->setStyleSheet(
		"QTextEdit { "
		"    background-color: #1e1e1e; "
		"    color: #cccccc; "
		"    border: none; "
		"    font-family: 'Monaco', 'Menlo', 'Consolas', monospace; "
		"    font-size: 12px; "
		"    padding: 4px; "
		"}" );
}

void ConsoleWidget::appendMessageInternal( int type, const QString &message ) {
	if ( !m_textEdit ) {
		return;
	}

	// Color based output based on the type that is being used
	QString color;
	QString prefix;

	switch ( type ) {
	case 0: // CONSOLE INFO
		color = "#cccccc";
		prefix = "";
		break;
	case 1: // CONSOLE SUCCESS
		color = "#4ec94e";
		prefix = "[OK]";
		break;
	case 2: // CONSOLE WARNING
		color = "#e6db74";
		prefix = "[!]";
        break;
	case 3: // CONSOLE ERROR
		color = "#f44747";
		prefix = "[ERROR]";
		break;
	default:
		color = "#cccccc";
		prefix = "";
		break;
	}

	QString htmlMessage = QString( "<span style=\"color: %1;\">%2 %3</span>" )
							  .arg( color )
							  .arg( prefix )
							  .arg( message.toHtmlEscaped() );

	m_textEdit->append( htmlMessage );
	// Auto Scroll to the bottom of the console panel
	QScrollBar *scrollBar = m_textEdit->verticalScrollBar();
	scrollBar->setValue( scrollBar->maximum() );
}

void ConsoleWidget::appendMessage( int type, const QString &message ) {
	if ( m_delayEnabled ) {
		appendMessageDelayed( type, message );
	} else {
		appendMessageInternal( type, message );
	}
}

void ConsoleWidget::appendMessageDelayed( int type, const QString &message ) {
	QueuedMessage msg;
	msg.type = type;
	msg.message = message;

	m_messageQueue.enqueue( msg );

	if ( !m_queueTimer->isActive() ) {
		m_queueTimer->start( m_delayInterval );
	}
}

void ConsoleWidget::processQueue() {
	if ( m_messageQueue.isEmpty() ) {
		return;
	}

	QueuedMessage msg = m_messageQueue.dequeue();
	appendMessageInternal( msg.type, msg.message );

	if ( !m_messageQueue.isEmpty() ) {
		m_queueTimer->start( m_delayInterval );
	}
}

void ConsoleWidget::clear( void ) {
	if ( m_textEdit ) {
		m_textEdit->clear();
	}
}

void ConsoleWidget::setDelayEnabled( bool enabled ) {
	m_delayEnabled = enabled;

	// If disabling, flush remaining queue
	if ( !enabled ) {
		flushQueue();
	}
}

void ConsoleWidget::setDelayInterval( int milliseconds ) {
	m_delayInterval = milliseconds;
}

void ConsoleWidget::flushQueue() {
	m_queueTimer->stop();
	while ( !m_messageQueue.isEmpty() ) {
		QueuedMessage msg = m_messageQueue.dequeue();
		appendMessageInternal( msg.type, msg.message );
	}
}

QString ConsoleWidget::getText( void ) const {
	return m_textEdit ? m_textEdit->toHtml() : QString();
}

QString ConsoleWidget::getPlainText( void ) const {
	return m_textEdit ? m_textEdit->toPlainText() : QString();
}
