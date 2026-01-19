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
 *   Purpose: Console Bridge used for bridging the C backend and C++ Qt Frontend
 * ═══════════════════════════════════════════════════════════════════════════
 */


#include "ConsoleBridge.h"
#include "ConsoleWidget.h"
#include "util_console.h"
#include <QString>


static ConsoleWidget *g_consoleWidget = nullptr;

extern "C" void console_bridge_callback( int type, const char *message ) {
    if ( g_consoleWidget && message ) {
        g_consoleWidget->appendMessage( type, QString::fromUtf8( message ) );
    } 
}


namespace ConsoleBridge {
    
    void init( ConsoleWidget *widget ) {
        
        g_consoleWidget = widget;
        
        console_set_callback( (console_callback_t)console_bridge_callback );
    }
    
    
    void shutdown( void ) {
        console_set_callback( nullptr );
        g_consoleWidget = nullptr;
    }
 
    bool isActive( void ) {
        return g_consoleWidget != nullptr;
    }
 
}
