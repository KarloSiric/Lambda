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
 *   Purpose: Factory functions for creating application menus
 * ===============================================================================
 */

#ifndef MENUFACTORY_H
#define MENUFACTORY_H

class QMainWindow;

namespace MenuFactory {

// Creates all menus and adds them to the main window's menu bar
void createMenus( QMainWindow *mainWindow );

// Individual menu creation functions
void createFileMenu( QMainWindow *mainWindow );
void createEditMenu( QMainWindow *mainWindow );
void createViewMenu( QMainWindow *mainWindow );
void createModelMenu( QMainWindow *mainWindow );
void createBodypartMenu( QMainWindow *mainWindow );
void createBonesMenu( QMainWindow *mainWindow );
void createSequencesMenu( QMainWindow *mainWindow );
void createTexturesMenu( QMainWindow *mainWindow );
void createToolsMenu( QMainWindow *mainWindow );
void createDebugMenu( QMainWindow *mainWindow );
void createWindowMenu( QMainWindow *mainWindow );
void createHelpMenu( QMainWindow *mainWindow );

} // namespace MenuFactory

#endif // MENUFACTORY_H
