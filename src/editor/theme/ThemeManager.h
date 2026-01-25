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

    // ═══════════════════════════════════════════════════════════════════════
    // THEME APPLICATION
    // ═══════════════════════════════════════════════════════════════════════

    static void applyClassicTheme( QApplication *app );   // Windows 95/2000 style (current default)
    static void applyModernLightTheme( QApplication *app ); // Modern light theme
    static void applyModernDarkTheme( QApplication *app );  // Modern dark theme (TODO)

    // ═══════════════════════════════════════════════════════════════════════
    // COMPONENT-SPECIFIC STYLESHEETS (Classic Theme)
    // ═══════════════════════════════════════════════════════════════════════

    static QString getClassicMainWindowStyle();    // Main window + MenuBar + Toolbar
    static QString getClassicTabWidgetStyle();      // Viewport tabs (with close buttons)
    static QString getClassicBottomTabsStyle();     // Console/Log/Memory tabs
    static QString getClassicDockStyle();           // Dock widgets
    static QString getClassicInspectorStyle();      // Inspector panel specific
    static QString getClassicViewportContainerStyle(); // Viewport container frame
};

#endif
