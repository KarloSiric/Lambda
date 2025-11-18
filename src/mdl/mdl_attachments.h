#ifndef ATTACHMENTS_H
#define ATTACHMENTS_H 


#include "studio.h"
#include "math/math_types.h"


#pragma once


// @Note(Karlo): First we need to be able to find the attachment by its name
int mdl_attachment_find( const studiohdr_t *header, const unsigned char *data, const char *name );

// @Note(Karlo): We get the attachment definition
const mstudioattachment_t *mdl_attachment_get_info( const studiohdr_t *header, const unsigned char *data, int index );




#endif
