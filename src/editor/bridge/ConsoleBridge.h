#ifndef CONSOLEBRIDGE_H
#define CONSOLEBRIDGE_H 

// Forward declaration
class ConsoleWidget;

namespace ConsoleBridge {
    
    void init( ConsoleWidget *widget );
    
    void shutdown( void );
    
    bool isActive( void );
    
} // namespace ConsoleBridge

#endif // CONSOLEBRIDGE_H
