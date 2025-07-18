#ifndef MDL_LOADER_H
#define MDL_LOADER_H 

#include <stddef.h>

typedef struct {
    float x, y, z;
} vector3_s;

/**
 * Structure based on the official Valve MDL file documentation (Valve SDK)
 */
typedef struct __attribute__((packed)) {
    int id;                                // IDST that can be seen using Hex Dump
    int version;                           // Format version
    char name[64];                         // Internal model name
    int dataLength;                        // Data size of MDL file in bytes

    vector3_s eyeposition;                 // Player viewpoint relative to model origin
    vector3_s illumposition;               // Position of ambient light calculation
    vector3_s hull_min;                    // Model hull box minimum corner
    vector3_s hull_max;                    // Model hull box max. corner
    vector3_s view_bbmin;                  // Bounding box min. for view culling
    vector3_s view_bbmax;                  // Bounding box max for view culling

    int flags;                             // Binary flags

    // Data needed for rendering
    int bone_count;
    int bone_offset;

    int bodypart_count;
    int bodypart_offset;


    // Gonna add more data as needed


} mdl_header_s;

typedef struct {
    float *vertices;
    size_t vertex_count;
    // TODO: Add faces, textures and etc..
} mdl_model_s;

mdl_model_s load_mdl_file(const char *filepath);
void free_mdl_file(mdl_model_s *model);
void print_mdl_info(const char *filepath);






#endif
