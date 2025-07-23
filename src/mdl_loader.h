#ifndef MDL_LOADER_H
#define MDL_LOADER_H 


#include <stddef.h>
#include <stdio.h>


#define STUDIO_VERSION       10
#define IDSTUDIOHEADER       (('T'<<24) + ('S'<<16) + ('D'<<8) + 'I')



typedef struct {
    float x, y, z;
} vec3_t;



// =================================
// STUDIO HEADER - Valve original struct
// =================================
typedef struct __attribute__((packed)) {
    int id;                                // IDST magic number
    int version;                           // Format version
    char name[64];                         // Internal model name
    int length;                            // Data size of MDL file in bytes

    vec3_t eyeposition;                    // ideal eye position
    vec3_t min;                            // ideal movement hull size
    vec3_t max;                            // ideal movement hull size
    vec3_t bbmin;                          // clipping bounding box
    vec3_t bbmax;                          // clipping bounding box

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

} studiohdr_t;



// =================================
// BODYPART - Valve original struct
// =================================
typedef struct {
    char name[64];                         // Bodypart name (e.g "body", "head")
    int nummodels;                         // Number of model variations
    int base;                              // Base value for model selection
    int modelindex;                        // Offset to model data array
} mstudiobodypart_t;



// =================================
// MODEL - Valve original struct
// =================================
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
} mstudiomodel_t;



typedef struct {
    float x, y, z;                         // 3D coordinates (converted from compressed format)
} mdl_vertex_s;



// =================================
// SINGLE MODEL
// =================================
typedef struct {
    float *vertices;                       // Flat array[x1,y1,z1, x2,y2,z2 ...]
    int vertex_count;                      // number of vertices per model
    int *triangle_indices;                 // Number of triangles that form the meshes
    int triangle_count;                    // number of triangles
    char model_name[64];                   // model name e.g. "EinsteinHead"
    char bodypart_name[64];                // bodypart name e.g. "heads", ...
    int bodypart_id;                       // bodypart id: 0, 1, 2 .. etc
    int model_id;                          // which variation of the bodypart 0, 1, 2 ...
} single_model_s;                



// =================================
// COMPLETE MODEL
// =================================
typedef struct {
    single_model_s *models;
    int total_model_count;
    char filename[64];
    int bodypart_count;
} mdl_complete_model_s;



// =================================
// MESH - Valve original struct
// =================================
typedef struct {
    int numtris;
    int triindex;
    int skinref;
    int numnorms;
    int normindex;
} mstudiomesh_t;



// =================================
// TRIANGLE VERTEX
// =================================
typedef struct {
    short vertindex;
    short normindex;
    short s, t;
} mstudiotrivert_t;



// =================================
// TRIANGLE READING
// =================================
typedef struct {
    mstudiotrivert_t triverts[3];
} triangle_data_t;


// =================================
// FUNCTION PROTOTYPES
// =================================


studiohdr_t *mdl_read_header(FILE *file);
mstudiobodypart_t *mdl_read_bodyparts(FILE *file, studiohdr_t *header);
mstudiomodel_t *mdl_read_models_for_bodyparts(FILE *file, mstudiobodypart_t *bodypart);
vec3_t *mdl_read_vertices(FILE *file, mstudiomodel_t *model);
triangle_data_t *mdl_read_triangles_for_models(FILE *file, mstudiomodel_t *model);






#endif
