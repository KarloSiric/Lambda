/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer
 * ═══════════════════════════════════════════════════════════════════════════
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
 * ───────────────────────────────────────────────────────────────────────────
 *   Author: Karlo Siric
 *   Purpose: Math utility functions for math library
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "math_types.h"


float Math_Clamp( float value, float min, float max ) {
    
    float temp;
    
    
    // 7, 5 -> 7 > 5 
    
    if ( min > max ) {
        temp = min;
        min = max;
        max = temp;
    }
    
    if ( value < min ) {
        value = min;
    }
    if ( value > max ) {
        value = max;
    }
     
    return value;    
}

float Math_Min( float a, float b ) {
    
    float min_value;
    
    if ( a < b ) {
        min_value = a;
    } else {
        min_value = b;
    }

    return min_value;
    
}

float Math_Max( float a, float b ) {
    
    float max_value;
    if ( a > b ) {
        max_value = a;
    } else {
        max_value = b;
    }
    
    return max_value; 
}

float Math_Lerp( float a, float b, float t ) {
    
    return ( a + ( t * ( b - a ) ) );
    
}
