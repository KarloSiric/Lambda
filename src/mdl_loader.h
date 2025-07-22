#ifndef MDL_LOADER_H
#define MDL_LOADER_H 

#include <stddef.h>

typedef struct {
    float x, y, z;
} vector3_s;

/**
 * @brief Structure EXACTLY matching Valve's official studio.h from GitHub
 *        Half-Life 1 MDL file header structure (studiohdr_t)
 */
typedef struct __attribute__((packed)) {
    int id;                                // IDST magic number
    int version;                           // Format version
    char name[64];                         // Internal model name
    int length;                            // Data size of MDL file in bytes

    vector3_s eyeposition;                 // ideal eye position
    vector3_s min;                         // ideal movement hull size
    vector3_s max;                         // ideal movement hull size
    vector3_s bbmin;                       // clipping bounding box
    vector3_s bbmax;                       // clipping bounding box

    int flags;                             // Binary flags

    int numbones;                          // bones
    int boneindex;                         // bone data offset

    int numbonecontrollers;                // bone controllers
    int bonecontrollerindex;               // bone controller data offset

    int numhitboxes;                       // complex bounding boxes
    int hitboxindex;                       // hitbox data offset

    int numseq;                            // animation sequences
    int seqindex;                          // sequence data offset

    int numseqgroups;                      // demand loaded sequences
    int seqgroupindex;                     // sequence group data offset

    int numtextures;                       // raw textures
    int textureindex;                      // texture data offset
    int texturedataindex;                  // raw texture data offset

    int numskinref;                        // replaceable textures
    int numskinfamilies;                   // skin families
    int skinindex;                         // skin data offset

    int numbodyparts;                      // *** KEY FIELD: bodyparts ***
    int bodypartindex;                     // *** KEY FIELD: bodypart data offset ***

    int numattachments;                    // queryable attachable points
    int attachmentindex;                   // attachment data offset

    int soundtable;                        // sound table
    int soundindex;                        // sound data offset
    int soundgroups;                       // sound groups
    int soundgroupindex;                   // sound group data offset

    int numtransitions;                    // animation transitions
    int transitionindex;                   // transition data offset

} mdl_header_s;

/**
 * @brief Bodypart structure - contains models which contain vertex data
 */
typedef struct {
    char name[64];                         // Bodypart name (e.g "body", "head")
    int nummodels;                         // Number of model variations
    int base;                              // Base value for model selection
    int modelindex;                        // Offset to model data array
} mdl_bodypart_s;

/**
 * @brief Model structure - this is where all mesh data lives!
 */
typedef struct {
    char name[64];                         // Model name
    int type;                              // Model type
    float boundingradius;                  // Bounding sphere radius
    int nummesh;                           // Number of meshes in this model
    int meshindex;                         // Offset to mesh data
    int numverts;                          // Number of unique vertices
    int vertinfoindex;                     // Offset to vertex bone data
    int vertindex;                         // *** VERTEX COORDINATES OFFSET! ***
    int numnorms;                          // Number of normals
    int norminfoindex;                     // Offset to normal bone info
    int normindex;                         // Offset to normal coordinates
    int numgroups;                         // Deformation groups
    int groupindex;                        // Offset to group data
} mdl_model_s;

/**
 * @brief Vertex structure - actual 3D coordinate data (vec3_t in Valve code)
 */
typedef struct {
    float x, y, z;                         // 3D coordinates (converted from compressed format)
} mdl_vertex_s;


/**
 * A single comprehensive model 
 * 
 * Contains all the data for a single model:
 * 
 * E.g. -> bodypart head, model name EinsteinHead 
 */
typedef struct {
    float *vertices;                       // Flat array[x1,y1,z1, x2,y2,z2 ...]
    int vertex_count;                      // number of vertices per model
    int *triangle_indices;        // Number of triangles that form the meshes
    int triangle_count;                    // number of triangles
    char model_name[64];                   // model name e.g. "EinsteinHead"
    char bodypart_name[64];                // bodypart name e.g. "heads", ...
    int bodypart_id;                       // bodypart id: 0, 1, 2 .. etc
    int model_id;                          // which variation of the bodypart 0, 1, 2 ...
} single_model_s;                

/**
 * One complete model - one full mdl file
 */
typedef struct {
    single_model_s *models;
    int total_model_count;
    char filename[64];
    int bodypart_count;
} mdl_complete_model_s;


/**
 * One mesh structure - consists of many triangles that forms the model
 */
typedef struct {
    int numtris;
    int triindex;
    int skinref;
    int numnorms;
    int normindex;
} mdl_mesh_s;


/**
 * Vertices indexes that form together one triangle for the meshes
 */

typedef struct {
    short vertindex[3];
} mdl_triangle_s;



#endif
