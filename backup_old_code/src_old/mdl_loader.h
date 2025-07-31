#ifndef MDL_LOADER_H
#define MDL_LOADER_H 


#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>


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



// =================================
// BONE - Valve original struct
// =================================
typedef struct {
    char name[32];                         // bone name for symbolic links
    int parent;                            // parent bone
    int flags;                             // ??
    int bonecontroller[6];                 // bone controller index, -1 == none
    float value[6];                        // default DoF values
    float scale[6];                        // scale for delta DoF values
} mstudiobone_t;



// =================================
// TEXTURE - Valve original struct  
// =================================
typedef struct {
    char name[64];
    int flags;
    int width;
    int height;
    int index;
} mstudiotexture_t;



typedef struct {
    float x, y, z;                         // 3D coordinates (converted from compressed format)
} mdl_vertex_s;



// =================================
// BONE WEIGHT DATA
// =================================
typedef struct mstudioboneweight_s {
    unsigned char bone_index;  // Which bone this vertex is attached to
} mstudioboneweight_t;

// =================================
// MESH RENDER DATA (Improved)
// =================================
typedef struct mdl_mesh_render_data_s {
    int *triangle_commands;     // Raw triangle command stream
    int command_count;          // Number of command integers
    float *vertices_flat;       // For OpenGL: [x1,y1,z1, x2,y2,z2, ...]
    float *texcoords_flat;      // For OpenGL: [s1,t1, s2,t2, ...]
    int *indices_flat;          // For OpenGL: [i1,i2,i3, i4,i5,i6, ...]
    int vertex_count;
    int texcoord_count;
    int index_count;
} mdl_mesh_render_data_s;

// =================================
// TEXTURE STRUCTURE
// =================================
typedef struct mdl_texture_s {
    char name[64];          // Texture name
    int flags;              // Rendering flags
    int width;              // Texture width
    int height;             // Texture height
    int index;              // Offset to texture data
    unsigned char *data;    // Raw texture data (8-bit indexed)
    unsigned char *palette; // 256 * 3 RGB palette
    unsigned int gl_texture_id; // OpenGL texture handle
} mdl_texture_t;

// =================================
// BONE TRANSFORMATION STATE
// =================================
typedef struct bone_transform_state_s {
    float bone_matrices[128][16]; // 4x4 matrices flattened
    int bone_count;
} bone_transform_state_t;

// =================================
// SINGLE MODEL (Enhanced)
// =================================
typedef struct {
    float *vertices;                       // Original vertices [x1,y1,z1, x2,y2,z2 ...]
    float *transformed_vertices;           // Vertices after bone transformation
    float *texcoords;                      // Texture coordinates [s1,t1, s2,t2 ...]
    int *triangle_indices;                 // Triangle indices
    mstudioboneweight_t *bone_weights;     // Which bone each vertex belongs to
    mdl_mesh_render_data_s *render_data;   // Processed render data
    bone_transform_state_t bone_state;     // Current bone transformations
    
    int vertex_count;                      // number of vertices per model
    int triangle_count;                    // number of triangles
    int texcoord_count;                    // number of texture coordinates
    
    char model_name[64];                   // model name e.g. "EinsteinHead"
    char bodypart_name[64];                // bodypart name e.g. "heads", ...
    int bodypart_id;                       // bodypart id: 0, 1, 2 .. etc
    int model_id;                          // which variation of the bodypart 0, 1, 2 ...
} single_model_s;                



// =================================
// COMPLETE MODEL (Enhanced)
// =================================
typedef struct {
    single_model_s *models;
    mstudiobone_t *bones;           // Bone hierarchy
    mdl_texture_t *textures;        // Loaded textures
    short *skin_families;           // Skin family data
    
    int total_model_count;
    int bone_count;
    int texture_count;
    int skin_family_count;
    int skin_ref_count;
    
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
// TRIANGLE VERTEX (Valve's actual format)
// =================================
typedef struct {
    short vertindex;      // index into vertex array
    short normindex;      // index into normal array
    short s, t;          // texture coordinates
} mstudiotrivert_t;




// =================================
// FUNCTION PROTOTYPES
// =================================


// Core loading functions
studiohdr_t *mdl_read_header(FILE *file);
mstudiobodypart_t *mdl_read_bodyparts(FILE *file, studiohdr_t *header);
mstudiomodel_t *mdl_read_models_for_bodyparts(FILE *file, mstudiobodypart_t *bodypart);
vec3_t *mdl_read_vertices(FILE *file, mstudiomodel_t *model);

// Enhanced triangle reading
mdl_mesh_render_data_s *mdl_read_triangle_commands(FILE *file, mstudiomodel_t *model);
void mdl_free_render_data(mdl_mesh_render_data_s *render_data);

// Bone system
mstudiobone_t *mdl_read_bones(FILE *file, studiohdr_t *header);
mstudioboneweight_t *mdl_read_vertex_bone_weights(FILE *file, mstudiomodel_t *model);
void mdl_setup_bones(bone_transform_state_t *state, mstudiobone_t *bones, int bone_count);
void mdl_transform_vertices_by_bones(bone_transform_state_t *bone_state, vec3_t *original_vertices, 
                                    int vertex_count, mstudioboneweight_t *bone_weights, float *output_vertices);

// Texture system
mdl_texture_t *mdl_read_textures(FILE *file, studiohdr_t *header);
unsigned char *mdl_convert_indexed_to_rgb(mdl_texture_t *texture);
unsigned int mdl_create_opengl_texture(unsigned char *rgb_data, int width, int height);
void mdl_free_textures(mdl_texture_t *textures, int count);

// Utility functions
long mdl_get_file_size(FILE *file);

// Main interface
mdl_complete_model_s *mdl_load_complete_file(const char *filename);
void mdl_free_complete_model(mdl_complete_model_s *complete_model);




#endif
