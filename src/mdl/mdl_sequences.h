#ifndef MDL_SEQUENCES_H
#define MDL_SEQUENCES_H

#include "studio.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


// Get sequence descriptor by index (returns NULL if invalid)
const mstudioseqdesc_t* mdl_sequence_get_info( const studiohdr_t *header,
                                                const unsigned char *data,
                                                int index );


// Get event from a sequence by event index (returns NULL if invalid)
const mstudioevent_t* mdl_sequence_get_event( const mstudioseqdesc_t *sequence,
                                               const unsigned char *data,
                                               int event_index );

/**
 * Print all sequences and their events for debugging
 * Shows: sequence name, fps, frame count, and all events with frame/ID/options
 *
 * @param header Model header
 * @param data Model data
 */
void mdl_sequence_dump( const studiohdr_t *header,
                        const unsigned char *data );

#ifdef __cplusplus
}
#endif

#endif // MDL_SEQUENCES_H
