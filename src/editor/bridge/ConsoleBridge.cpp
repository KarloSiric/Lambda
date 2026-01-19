/*======================================================================
   File: ConsoleBridge.cpp
   Project: ModelViewer
   Author: karlosiric <email@example.com>
   Created: 2026-01-19 12:06:17
   Last Modified by: karlosiric
   Last Modified: 2026-01-19 12:13:15
   ---------------------------------------------------------------------
   Description:
       
   ---------------------------------------------------------------------
   License: 
   Company: 
   Version: 0.1.0
 ======================================================================
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
