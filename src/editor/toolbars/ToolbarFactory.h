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
 *   Purpose: Factory functions for creating application toolbars
 * ===============================================================================
 */

#ifndef TOOLBARFACTORY_H
#define TOOLBARFACTORY_H

class QMainWindow;

namespace ToolbarFactory {

// Creates all toolbars and adds them to the main window
void createToolbars( QMainWindow *mainWindow );

// Individual toolbar creation functions
void createMainToolbar( QMainWindow *mainWindow );
void createSecondaryToolbar( QMainWindow *mainWindow );

} // namespace ToolbarFactory

#endif // TOOLBARFACTORY_H
