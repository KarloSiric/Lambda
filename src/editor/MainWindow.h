#ifndef MAINWINDOW_H
#define MAINWINDOW_H 

#pragma once

#include <QtCore/qobject.h>
#include <QtWidgets/qwidget.h>
#include <QtCore/qtmetamacros.h>
#include <QtWidgets/qmainwindow.h>
#include <QMainWindow>

class MainWindow : public QMainWindow 
{
    Q_OBJECT
    
public:
    
    explicit MainWindow( QWidget *parent = nullptr );
    
private:
    
    void setupMenus();
    void setupToolbars();
    void setupLeftToolbox();
    void setupRightInspector();
    void setupViewports();
    
};

#endif
