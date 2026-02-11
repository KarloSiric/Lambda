/*
 * ===============================================================================
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ===============================================================================
 *
 *   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 * -------------------------------------------------------------------------------
 *   Author: Karlo Siric
 *   Purpose: Programmatic icon generation for consistent toolbar appearance
 * ===============================================================================
 */

#ifndef ICONFACTORY_H
#define ICONFACTORY_H

#include <QIcon>

namespace IconFactory {

// All icons are 32x32, flat style, dark on transparent background

QIcon wireframe();
QIcon textured();
QIcon bones();
QIcon hitboxes();
QIcon attachments();
QIcon grid();
QIcon axes();
QIcon ground();
QIcon cameraReset();
QIcon viewFront();
QIcon viewTop();
QIcon viewSide();
QIcon gridIncrease();
QIcon gridDecrease();

} // namespace IconFactory

#endif // ICONFACTORY_H
