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
    m_textEdit( nullptr ) {
        setupUI();
        applyStyles();
}

ConsoleWidget::~ConsoleWidget() {
    // DESTRUCTOR
    
    
}


void ConsoleWidget::setupUI( void )
{
    QVBoxLayout *v_layout = new QVBoxLayout( this );
    v_layout->setContentsMargins( 0, 0, 0, 0 );
    v_layout->setSpacing( 0 );
    
    m_textEdit = new QTextEdit();
    m_textEdit->setReadOnly( true );
    m_textEdit->setLineWrapMode( QTextEdit::NoWrap );
    m_textEdit->setAcceptRichText( true );
    
    v_layout->addWidget( m_textEdit );
    
}


void ConsoleWidget::applyStyles( void )
{
    
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
    

void ConsoleWidget::appendMessage( int type, const QString &message )
{
    
    if ( !m_textEdit ) {
        return ;
    }   
    
    // Color based output based on the type that is being used
    QString color;
    QString prefix;
    
    switch( type ) {
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
        case 3:
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

void ConsoleWidget::clear( void )
{
    if ( m_textEdit ) {
        m_textEdit->clear();
    }
}
 
QString ConsoleWidget::getText( void ) const
{
    return m_textEdit ? m_textEdit->toHtml() : QString();
    
}   

QString ConsoleWidget::getPlainText( void ) const
{
    return m_textEdit ? m_textEdit->toPlainText() : QString();
}
