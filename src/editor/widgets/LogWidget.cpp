/*
 * ===============================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===============================================================================
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
 * -------------------------------------------------------------------------------
 *   Author: Karlo Siric
 *   Purpose: Log Widget implementation with filtering and log levels
 * ===============================================================================
 */

#include "LogWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QClipboard>
#include <QApplication>
#include <QLabel>

LogWidget::LogWidget( QWidget *parent )
	: QWidget( parent ),
	  m_logView( nullptr ),
	  m_levelFilter( nullptr ),
	  m_searchFilter( nullptr ),
	  m_autoScrollCheck( nullptr ),
	  m_timestampCheck( nullptr ),
	  m_clearBtn( nullptr ),
	  m_copyBtn( nullptr ),
	  m_saveBtn( nullptr ),
	  m_minLevel( LogLevel::Trace ),
	  m_autoScroll( true ),
	  m_showTimestamp( true ) {

	setupUI();
	applyStyles();

	// Setup flush timer for batched logging
	m_flushTimer = new QTimer( this );
	m_flushTimer->setSingleShot( true );
	connect( m_flushTimer, &QTimer::timeout, this, &LogWidget::flushLogQueue );
}

LogWidget::~LogWidget() {
}

void LogWidget::setupUI() {
	QVBoxLayout *mainLayout = new QVBoxLayout( this );
	mainLayout->setContentsMargins( 0, 0, 0, 0 );
	mainLayout->setSpacing( 0 );

	// Toolbar row
	QWidget *toolbar = new QWidget();
	QHBoxLayout *toolbarLayout = new QHBoxLayout( toolbar );
	toolbarLayout->setContentsMargins( 4, 2, 4, 2 );
	toolbarLayout->setSpacing( 4 );

	// Level filter dropdown
	QLabel *levelLabel = new QLabel( "Level:" );
	m_levelFilter = new QComboBox();
	m_levelFilter->addItem( "Trace", static_cast<int>( LogLevel::Trace ) );
	m_levelFilter->addItem( "Debug", static_cast<int>( LogLevel::Debug ) );
	m_levelFilter->addItem( "Info", static_cast<int>( LogLevel::Info ) );
	m_levelFilter->addItem( "Warning", static_cast<int>( LogLevel::Warning ) );
	m_levelFilter->addItem( "Error", static_cast<int>( LogLevel::Error ) );
	m_levelFilter->addItem( "Fatal", static_cast<int>( LogLevel::Fatal ) );
	m_levelFilter->setCurrentIndex( 0 ); // Show all by default
	connect( m_levelFilter, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, &LogWidget::onFilterChanged );

	// Search filter - searches in message text
	QLabel *searchLabel = new QLabel( "Search:" );
	m_searchFilter = new QLineEdit();
	m_searchFilter->setPlaceholderText( "Filter text..." );
	m_searchFilter->setMaximumWidth( 150 );
	connect( m_searchFilter, &QLineEdit::textChanged, this, &LogWidget::onFilterChanged );

	// Options
	m_autoScrollCheck = new QCheckBox( "Auto-scroll" );
	m_autoScrollCheck->setChecked( true );
	connect( m_autoScrollCheck, &QCheckBox::toggled, this, [this]( bool checked ) {
		m_autoScroll = checked;
	} );

	m_timestampCheck = new QCheckBox( "Timestamps" );
	m_timestampCheck->setChecked( true );
	connect( m_timestampCheck, &QCheckBox::toggled, this, [this]( bool checked ) {
		m_showTimestamp = checked;
		refreshDisplay();
	} );

	// Buttons
	m_clearBtn = new QPushButton( "Clear" );
	m_copyBtn = new QPushButton( "Copy" );
	m_saveBtn = new QPushButton( "Save" );
	connect( m_clearBtn, &QPushButton::clicked, this, &LogWidget::onClearClicked );
	connect( m_copyBtn, &QPushButton::clicked, this, &LogWidget::onCopyClicked );
	connect( m_saveBtn, &QPushButton::clicked, this, &LogWidget::onSaveClicked );

	// Add to toolbar
	toolbarLayout->addWidget( levelLabel );
	toolbarLayout->addWidget( m_levelFilter );
	toolbarLayout->addSpacing( 8 );
	toolbarLayout->addWidget( searchLabel );
	toolbarLayout->addWidget( m_searchFilter );
	toolbarLayout->addSpacing( 8 );
	toolbarLayout->addWidget( m_autoScrollCheck );
	toolbarLayout->addWidget( m_timestampCheck );
	toolbarLayout->addStretch();
	toolbarLayout->addWidget( m_clearBtn );
	toolbarLayout->addWidget( m_copyBtn );
	toolbarLayout->addWidget( m_saveBtn );

	// Log view
	m_logView = new QTextEdit();
	m_logView->setReadOnly( true );
	m_logView->setLineWrapMode( QTextEdit::NoWrap );
	m_logView->setAcceptRichText( true );

	mainLayout->addWidget( toolbar );
	mainLayout->addWidget( m_logView );
}

void LogWidget::applyStyles() {
	// Classic Windows 95/98 style - match the rest of the app
	setStyleSheet( "QWidget { background-color: #c0c0c0; }" );

	// Combobox style
	QString comboStyle =
		"QComboBox { "
		"    background-color: #ffffff; "
		"    color: #000000; "
		"    border: 2px solid; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #ffffff; "
		"    border-bottom-color: #ffffff; "
		"    padding: 1px 4px; "
		"    font-size: 11px; "
		"} "
		"QComboBox QAbstractItemView { "
		"    background-color: #ffffff; "
		"    color: #000000; "
		"    selection-background-color: #000080; "
		"    selection-color: #ffffff; "
		"}";

	// Line edit style
	QString lineEditStyle =
		"QLineEdit { "
		"    background-color: #ffffff; "
		"    color: #000000; "
		"    border: 2px solid; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #ffffff; "
		"    border-bottom-color: #ffffff; "
		"    padding: 1px 4px; "
		"    font-size: 11px; "
		"}";

	// Button style
	QString buttonStyle =
		"QPushButton { "
		"    background-color: #c0c0c0; "
		"    color: #000000; "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"    padding: 2px 8px; "
		"    font-size: 11px; "
		"} "
		"QPushButton:pressed { "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #ffffff; "
		"    border-bottom-color: #ffffff; "
		"}";

	// Checkbox style - use native Qt checkmark by not overriding indicator:checked image
	QString checkboxStyle =
		"QCheckBox { "
		"    color: #000000; "
		"    spacing: 4px; "
		"    font-size: 11px; "
		"} "
		"QCheckBox::indicator { "
		"    width: 13px; "
		"    height: 13px; "
		"} "
		"QCheckBox::indicator:unchecked { "
		"    background-color: #ffffff; "
		"    border: 2px solid; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #ffffff; "
		"    border-bottom-color: #ffffff; "
		"} "
		"QCheckBox::indicator:checked { "
		"    background-color: #000080; "
		"    border: 2px solid; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #ffffff; "
		"    border-bottom-color: #ffffff; "
		"}";

	// Label style
	QString labelStyle = "QLabel { color: #000000; background: transparent; }";

	m_levelFilter->setStyleSheet( comboStyle );
	m_searchFilter->setStyleSheet( lineEditStyle );
	m_clearBtn->setStyleSheet( buttonStyle );
	m_copyBtn->setStyleSheet( buttonStyle );
	m_saveBtn->setStyleSheet( buttonStyle );
	m_autoScrollCheck->setStyleSheet( checkboxStyle );
	m_timestampCheck->setStyleSheet( checkboxStyle );

	// Find and style the labels
	for ( QLabel *label : findChildren<QLabel *>() ) {
		label->setStyleSheet( labelStyle );
	}

	// Log view - dark sunken area
	m_logView->setStyleSheet(
		"QTextEdit { "
		"    background-color: #1e1e1e; "
		"    color: #cccccc; "
		"    border: 2px solid; "
		"    border-top-color: #808080; "
		"    border-left-color: #808080; "
		"    border-right-color: #ffffff; "
		"    border-bottom-color: #ffffff; "
		"    font-family: 'Monaco', 'Menlo', 'Consolas', monospace; "
		"    font-size: 13px; "
		"    padding: 4px; "
		"} "
		"QScrollBar:vertical { "
		"    background-color: #c0c0c0; "
		"    width: 16px; "
		"} "
		"QScrollBar::handle:vertical { "
		"    background-color: #c0c0c0; "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"    min-height: 20px; "
		"} "
		"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { "
		"    background-color: #c0c0c0; "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"    height: 16px; "
		"} "
		"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { "
		"    background-color: #808080; "
		"} "
		"QScrollBar:horizontal { "
		"    background-color: #c0c0c0; "
		"    height: 16px; "
		"} "
		"QScrollBar::handle:horizontal { "
		"    background-color: #c0c0c0; "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"    min-width: 20px; "
		"} "
		"QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { "
		"    background-color: #c0c0c0; "
		"    border: 2px solid; "
		"    border-top-color: #ffffff; "
		"    border-left-color: #ffffff; "
		"    border-right-color: #808080; "
		"    border-bottom-color: #808080; "
		"    width: 16px; "
		"} "
		"QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { "
		"    background-color: #808080; "
		"}" );
}

void LogWidget::log( LogLevel level, const QString &message, const QString &category ) {
	LogEntry entry;
	entry.level = level;
	entry.message = message;
	entry.category = category;
	entry.timestamp = QDateTime::currentDateTime();

	m_logQueue.enqueue( entry );

	if ( !m_flushTimer->isActive() ) {
		m_flushTimer->start( FLUSH_INTERVAL );
	}
}

void LogWidget::flushLogQueue() {
	while ( !m_logQueue.isEmpty() ) {
		LogEntry entry = m_logQueue.dequeue();

		// Store ALL logs for later filtering
		m_allLogs.append( entry );

		// Limit stored logs
		while ( m_allLogs.size() > MAX_LOG_ENTRIES ) {
			m_allLogs.removeFirst();
		}

		// Display if matches current filter
		if ( shouldDisplay( entry.level, entry.message, entry.category ) ) {
			appendLogEntry( entry.level, entry.message, entry.category, entry.timestamp );
		}
	}
}

void LogWidget::refreshDisplay() {
	// Clear display and re-render all logs that match current filter
	if ( !m_logView ) return;

	m_logView->clear();

	for ( const LogEntry &entry : m_allLogs ) {
		if ( shouldDisplay( entry.level, entry.message, entry.category ) ) {
			appendLogEntry( entry.level, entry.message, entry.category, entry.timestamp );
		}
	}
}

void LogWidget::appendLogEntry( LogLevel level, const QString &message, const QString &category, const QDateTime &timestamp ) {
	if ( !m_logView ) return;

	QString color = levelToColor( level );
	QString levelStr = levelToString( level );

	QString html;

	// Timestamp
	if ( m_showTimestamp ) {
		html += QString( "<span style=\"color: #666666;\">[%1]</span> " )
					.arg( timestamp.toString( "hh:mm:ss.zzz" ) );
	}

	// Level badge
	html += QString( "<span style=\"color: %1; font-weight: bold;\">[%2]</span> " )
				.arg( color )
				.arg( levelStr );

	// Category
	if ( !category.isEmpty() ) {
		html += QString( "<span style=\"color: #888888;\">[%1]</span> " )
					.arg( category.toHtmlEscaped() );
	}

	// Message
	html += QString( "<span style=\"color: %1;\">%2</span>" )
				.arg( level >= LogLevel::Warning ? color : "#cccccc" )
				.arg( message.toHtmlEscaped() );

	m_logView->append( html );

	// Auto scroll
	if ( m_autoScroll ) {
		QScrollBar *scrollBar = m_logView->verticalScrollBar();
		scrollBar->setValue( scrollBar->maximum() );
	}
}

bool LogWidget::shouldDisplay( LogLevel level, const QString &message, const QString &category ) const {
	// Check level filter
	if ( level < m_minLevel ) return false;

	// Check search filter - search in both message and category
	QString filter = m_searchFilter ? m_searchFilter->text() : "";
	if ( !filter.isEmpty() ) {
		bool foundInMessage = message.contains( filter, Qt::CaseInsensitive );
		bool foundInCategory = category.contains( filter, Qt::CaseInsensitive );
		if ( !foundInMessage && !foundInCategory ) {
			return false;
		}
	}

	return true;
}

QString LogWidget::levelToString( LogLevel level ) const {
	switch ( level ) {
		case LogLevel::Trace:   return "TRACE";
		case LogLevel::Debug:   return "DEBUG";
		case LogLevel::Info:    return "INFO";
		case LogLevel::Warning: return "WARN";
		case LogLevel::Error:   return "ERROR";
		case LogLevel::Fatal:   return "FATAL";
		default:                return "???";
	}
}

QString LogWidget::levelToColor( LogLevel level ) const {
	switch ( level ) {
		case LogLevel::Trace:   return "#666666";
		case LogLevel::Debug:   return "#888888";
		case LogLevel::Info:    return "#4ec94e";
		case LogLevel::Warning: return "#e6db74";
		case LogLevel::Error:   return "#f44747";
		case LogLevel::Fatal:   return "#ff0000";
		default:                return "#cccccc";
	}
}

// Convenience methods
void LogWidget::trace( const QString &message, const QString &category ) {
	log( LogLevel::Trace, message, category );
}

void LogWidget::debug( const QString &message, const QString &category ) {
	log( LogLevel::Debug, message, category );
}

void LogWidget::info( const QString &message, const QString &category ) {
	log( LogLevel::Info, message, category );
}

void LogWidget::warning( const QString &message, const QString &category ) {
	log( LogLevel::Warning, message, category );
}

void LogWidget::error( const QString &message, const QString &category ) {
	log( LogLevel::Error, message, category );
}

void LogWidget::fatal( const QString &message, const QString &category ) {
	log( LogLevel::Fatal, message, category );
}

void LogWidget::clear() {
	if ( m_logView ) {
		m_logView->clear();
	}
	m_logQueue.clear();
	m_allLogs.clear();
}

void LogWidget::setMinLevel( LogLevel level ) {
	m_minLevel = level;
	if ( m_levelFilter ) {
		m_levelFilter->setCurrentIndex( static_cast<int>( level ) );
	}
}

void LogWidget::setAutoScroll( bool enabled ) {
	m_autoScroll = enabled;
	if ( m_autoScrollCheck ) {
		m_autoScrollCheck->setChecked( enabled );
	}
}

void LogWidget::setTimestampEnabled( bool enabled ) {
	m_showTimestamp = enabled;
	if ( m_timestampCheck ) {
		m_timestampCheck->setChecked( enabled );
	}
}

QString LogWidget::getPlainText() const {
	return m_logView ? m_logView->toPlainText() : QString();
}

QString LogWidget::getHtml() const {
	return m_logView ? m_logView->toHtml() : QString();
}

void LogWidget::onFilterChanged() {
	m_minLevel = static_cast<LogLevel>( m_levelFilter->currentData().toInt() );
	// Refresh display with new filter
	refreshDisplay();
}

void LogWidget::onClearClicked() {
	clear();
}

void LogWidget::onCopyClicked() {
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText( getPlainText() );
}

void LogWidget::onSaveClicked() {
	QString fileName = QFileDialog::getSaveFileName(
		this,
		"Save Log",
		"lambda_log.txt",
		"Text Files (*.txt);;All Files (*)"
	);

	if ( fileName.isEmpty() ) return;

	QFile file( fileName );
	if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
		QTextStream stream( &file );
		stream << getPlainText();
		file.close();
	}
}
