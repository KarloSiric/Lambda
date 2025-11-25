#ifndef MAINWINDOW_H
#define MAINWINDOW_H 

#pragma once

#include <QMainWindow>


#define MW_WIDTH               1600
#define MW_HEIGHT              900

class MainWindow : public QMainWindow 
{
    Q_OBJECT
    
public:
    
    explicit MainWindow( QWidget *parent = nullptr );
    
private:
    
    void setupMenus( );
    void setupToolbars( );
    void setupLeftToolbox( );
    void setupRightInspector( );
    void setupViewports( );
    
};

#endif
