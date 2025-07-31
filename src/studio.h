/*
==============================================================================

Studio Model Format - Official Valve SDK Headers
Half-Life 1 Studio Model Format (Version 10)

Based on Valve's official Half-Life SDK
This is the EXACT format used by the game engine

==============================================================================
*/

#ifndef STUDIO_H
#define STUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Studio model limits
#define MAXSTUDIOTRIANGLES  20000   // max triangles per model
#define MAXSTUDIOVERTS      2048    // max vertices per model  
#define MAXSTUDIOSEQUENCES  2048    // total animation sequences
#define MAXSTUDIOSKINS      100     // total textures
#define MAXSTUDIOSRCBONES   512     // bones allowed at source movement
#define MAXSTUDIOBONES      128     // total bones actually used
#define MAXSTUDIOMODELS     32      // sub-models per model
#define MAXSTUDIOBODYPARTS  32
#define MAXSTUDIOGROUPS     16
#define MAXSTUDIOANIMATIONS 2048
#define MAXSTUDIOMESHES     256
#define MAXSTUDIOEVENTS     1024
#define MAXSTUDIOPIVOTS     256
#define MAXSTUDIOCONTROLLERS 8

// Studio model format constants
#define STUDIO_VERSION      10
#define IDSTUDIOHEADER      (('T'<<24)+('S'<<16)+('D'<<8)+'I')  // "IDST"
#define IDSEQGRPHEADER      (('Q'<<24)+('S'<<16)+('D'<<8)+'I')  // "IDSQ"

typedef float vec3_t[3];
typedef float vec4_t[4];
typedef unsigned char byte;

// ============================================================================
// MAIN STUDIO HEADER - This is the root structure of any .mdl file
// ============================================================================
typedef struct
{
    int                 id;                     // Model format identifier: "IDST" (0x54534449)
    int                 version;                // Format version (should be 10)
    
    char                name[64];               // Model name
    int                 length;                 // Model file size
    
    vec3_t              eyeposition;            // Ideal eye position
    vec3_t              min;                    // Ideal movement hull size
    vec3_t              max;                    
    
    vec3_t              bbmin;                  // Clipping bounding box
    vec3_t              bbmax;
    
    int                 flags;                  // Model flags
    
    int                 numbones;               // bones
    int                 boneindex;
    
    int                 numbonecontrollers;     // bone controllers
    int                 bonecontrollerindex;
    
    int                 numhitboxes;            // complex bounding boxes
    int                 hitboxindex;            
    
    int                 numseq;                 // animation sequences
    int                 seqindex;
    
    int                 numseqgroups;           // demand loaded sequences
    int                 seqgroupindex;
    
    int                 numtextures;            // raw textures
    int                 textureindex;
    int                 texturedataindex;
    
    int                 numskinref;             // replaceable textures
    int                 numskinfamilies;
    int                 skinindex;
    
    int                 numbodyparts;           // bodypart data
    int                 bodypartindex;
    
    int                 numattachments;         // queryable attachable points
    int                 attachmentindex;
    
    int                 soundtable;
    int                 soundindex;
    int                 soundgroups;
    int                 soundgroupindex;
    
    int                 numtransitions;         // animation node to animation node transition graph
    int                 transitionindex;
} studiohdr_t;

// ============================================================================
// BODYPARTS - Groups of models for different configurations
// ============================================================================
typedef struct
{
    char                name[64];
    int                 nummodels;
    int                 base;
    int                 modelindex;             // index into models array
} mstudiobodypart_t;

// ============================================================================
// MODELS - Individual mesh within a bodypart
// ============================================================================
typedef struct
{
    char                name[64];
    
    int                 type;
    
    float               boundingradius;
    
    int                 nummesh;                // Number of meshes
    int                 meshindex;              // Index of first mesh
    
    int                 numverts;               // number of unique vertices/normals/texcoords
    int                 vertinfoindex;          // vertex bone info
    int                 vertindex;              // vertex vec3_t
    int                 numnorms;               // number of unique surface normals
    int                 norminfoindex;          // normal bone info
    int                 normindex;              // normal vec3_t
    
    int                 numgroups;              // deformation groups
    int                 groupindex;
} mstudiomodel_t;

// ============================================================================
// MESHES - Triangle mesh within a model
// ============================================================================
typedef struct
{
    int                 numtris;                // Number of triangles
    int                 triindex;               // Index to triangle commands
    int                 skinref;                // material
    int                 numnorms;               // per mesh normals
    int                 normindex;              // normal vec3_t
} mstudiomesh_t;

// ============================================================================
// TRIANGLE VERTEX - Individual vertex in triangle strip/fan
// ============================================================================
typedef struct
{
    short               vertindex;             // index into vertex array
    short               normalindex;           // index into normal array
    short               s, t;                  // skin texture coords
} mstudiotrivert_t;

// ============================================================================
// TEXTURES - Material information
// ============================================================================
typedef struct
{
    char                name[64];
    int                 flags;
    int                 width, height;
    int                 index;                  // index into texture data
} mstudiotexture_t;

// ============================================================================
// BONES - Skeletal structure
// ============================================================================
typedef struct
{
    char                name[32];               // bone name for symbolic links
    int                 parent;                 // parent bone
    int                 flags;                  // ??
    int                 bonecontroller[6];      // bone controller index, -1 == none
    float               value[6];               // default DoF values
    float               scale[6];               // scale for delta DoF values
} mstudiobone_t;

// ============================================================================
// TRIANGLE COMMAND TYPES
// ============================================================================
#define STUDIO_RENDER_TRIANGLES     0
#define STUDIO_RENDER_TRIANGLE_STRIP 1
#define STUDIO_RENDER_TRIANGLE_FAN  2

#ifdef __cplusplus
}
#endif

#endif // STUDIO_H
