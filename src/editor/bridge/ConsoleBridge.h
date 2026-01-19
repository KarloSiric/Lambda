#ifndef CONSOLEBRIDGE_H
#define CONSOLEBRIDGE_H 

// Forward declaration, dont need to include 100 lines of code for nothing
class ConsoleWidget;

namespace ConsoleBridge {
    
    void init( ConsoleWidget *widget );
    
    void shutdown( void );
    
    bool isActive( void );
    
} // namespace ConsoleBridge

#endif // CONSOLEBRIDGE_H
