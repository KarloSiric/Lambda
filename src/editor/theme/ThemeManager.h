#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H 

#include <QObject>
#include <QApplication>
#include <QString>
#include <QtCore/qtmetamacros.h>
#include <QtWidgets/qapplication.h>


class ThemeManager : public QObject
{
    Q_OBJECT
    
public:
    
    static void applyThemeLight( QApplication &app );
    static void applyThemeDark( QApplication &app );
};

#endif
