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
 *   Purpose: Log Widget with filtering, timestamps, and log levels
 * ===============================================================================
 */

#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QQueue>
#include <QDateTime>

// Log levels matching C backend
enum class LogLevel {
	Trace = 0,
	Debug = 1,
	Info = 2,
	Warning = 3,
	Error = 4,
	Fatal = 5
};

class LogWidget : public QWidget {

	Q_OBJECT

  public:

	explicit LogWidget( QWidget *parent = nullptr );
	~LogWidget();

	// Log message with level and optional category
	void log( LogLevel level, const QString &message, const QString &category = "" );

	// Convenience methods
	void trace( const QString &message, const QString &category = "" );
	void debug( const QString &message, const QString &category = "" );
	void info( const QString &message, const QString &category = "" );
	void warning( const QString &message, const QString &category = "" );
	void error( const QString &message, const QString &category = "" );
	void fatal( const QString &message, const QString &category = "" );

	// Control
	void clear();
	void setMinLevel( LogLevel level );
	void setAutoScroll( bool enabled );
	void setTimestampEnabled( bool enabled );

	// Get log contents
	QString getPlainText() const;
	QString getHtml() const;

  public slots:

	void onFilterChanged();
	void onClearClicked();
	void onCopyClicked();
	void onSaveClicked();

  private:

	void setupUI();
	void applyStyles();
	void appendLogEntry( LogLevel level, const QString &message, const QString &category, const QDateTime &timestamp );
	QString levelToString( LogLevel level ) const;
	QString levelToColor( LogLevel level ) const;
	bool shouldDisplay( LogLevel level, const QString &category ) const;

	struct LogEntry {
		LogLevel level;
		QString message;
		QString category;
		QDateTime timestamp;
	};

	// UI Components
	QTextEdit *m_logView;
	QComboBox *m_levelFilter;
	QLineEdit *m_searchFilter;
	QCheckBox *m_autoScrollCheck;
	QCheckBox *m_timestampCheck;
	QPushButton *m_clearBtn;
	QPushButton *m_copyBtn;
	QPushButton *m_saveBtn;

	// State
	LogLevel m_minLevel;
	bool m_autoScroll;
	bool m_showTimestamp;
	QString m_categoryFilter;

	// Message queue for batching
	QQueue<LogEntry> m_logQueue;
	QTimer *m_flushTimer;
	static const int FLUSH_INTERVAL = 50; // ms
	static const int MAX_LOG_LINES = 10000;

  private slots:

	void flushLogQueue();
};

#endif // LOGWIDGET_H
