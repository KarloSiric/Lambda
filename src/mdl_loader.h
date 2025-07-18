#ifndef MDL_LOADER_H
#define MDL_LOADER_H 

#include <stddef.h>

typedef struct {
    float x, y, z;
} vector3_s;

/**
 * 
 * @brief Structure based on the official Valve MDL file documentation (Valve SDK)
 *        This represents the complete header structure for the Half-Life 1 MDL files
 * 
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

    int bone_count;
    int bone_offset;

    int bodypart_count;
    int bodypart_offset;


    int bonecontroller_count;              // Number of bone controllers
    int bonecontroller_offset;             // Offset to bone controller data
    int hitbox_count;                      // Number of collision boxed
    int hitbox_offset;                     // Offset to hitbox data


    int seq_count;                         // Animation sequences
    int seq_offset;
    int seqgroup_count;                    // Sequence groups
    int seqgroup_offset;
    int texture_count;                     // Texture count
    int texture_offset;                    // texture data offset
    int texturedata_offset;                // Texture raw data offset

    int skin_count;                        // skin variations
    int skin_offset;                       

    int attachment_count;                  // Attachment points
    int attachment_offset;


} mdl_header_s;



/**
 * @brief This represents the structure that holds all the bodypart data
 *        Contains the actual vertex data that will be needed.
 * 
 * 
 */
typedef struct {
    char name[64];                         // Bodypart name (e.g "body", "head")
    int nummodels;                         // Number of model variations
    int base;                              // Base value for model selectio
    int modelindex;                        // Offset to model data array

} mdl_bodypart_s;



/**
 * @brief Model structure - this is where all of the mesh data lives!
 * 
 */
typedef struct {
    char name[64];
    int type;
    float boundingradius;
    





}



typedef struct {
    float *vertices;
    size_t vertex_count;
    // TODO: Add faces, textures and etc..
} mdl_model_s;

mdl_model_s load_mdl_file(const char *filepath);
void free_mdl_file(mdl_model_s *model);
void print_mdl_info(const char *filepath);






#endif
