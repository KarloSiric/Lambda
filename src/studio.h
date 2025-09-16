/*
==============================================================================

studio.h - Official Valve Half-Life Studio Model Format
Version 10 - Half-Life 1 / GoldSource Engine

This is based on Valve's official Half-Life SDK headers.
This exact format is used by the game engine.

Key structures:
- studiohdr_t: Main file header (starts every .mdl file)
- mstudiobodypart_t: Groups of models for different configurations  
- mstudiomodel_t: Individual mesh within a bodypart
- mstudiomesh_t: Triangle mesh within a model
- mstudiotrivert_t: Individual vertex in triangle strip/fan
- mstudiotexture_t: Texture/material information
- mstudiobone_t: Skeletal bone data

==============================================================================
*/

#ifndef STUDIO_H
#define STUDIO_H
#ifdef __cplusplus

extern "C" {
#endif

#include <stdint.h>

// ============================================================================
// STUDIO MODEL CONSTANTS
// ============================================================================

// Format identification
#define STUDIO_VERSION      10
#define IDSTUDIOHEADER      (('T'<<24)+('S'<<16)+('D'<<8)+'I')  // "IDST" 
#define IDSEQGRPHEADER      (('Q'<<24)+('S'<<16)+('D'<<8)+'I')  // "IDSQ"

// Model limits (from Valve's code)
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

// Type definitions
typedef float vec3_t[3];
typedef float vec4_t[4];
typedef unsigned char byte;

// ============================================================================
// MAIN STUDIO HEADER
// This is the first thing in every .mdl file
// ============================================================================
typedef struct
{
    int                 id;                     // Model format ID: "IDST" (0x54534449)
    int                 version;                // Format version (should be 10)
    
    char                name[64];               // Model name
    int                 length;                 // Model file size in bytes
    
    vec3_t              eyeposition;            // Ideal eye position
    vec3_t              min;                    // Ideal movement hull size
    vec3_t              max;                    
    
    vec3_t              bbmin;                  // Clipping bounding box
    vec3_t              bbmax;
    
    int                 flags;                  // Model flags
    
    // Bones
    int                 numbones;               // Number of bones
    int                 boneindex;              // Offset to bone data
    
    // Bone controllers
    int                 numbonecontrollers;     // Number of bone controllers
    int                 bonecontrollerindex;    // Offset to bone controller data
    
    // Hit boxes
    int                 numhitboxes;            // Number of complex bounding boxes
    int                 hitboxindex;            // Offset to hitbox data
    
    // Animations
    int                 numseq;                 // Number of animation sequences
    int                 seqindex;               // Offset to sequence data
    
    // Animation groups
    int                 numseqgroups;           // Number of demand loaded sequences
    int                 seqgroupindex;          // Offset to sequence group data
    
    // Textures
    int                 numtextures;            // Number of raw textures
    int                 textureindex;           // Offset to texture data
    int                 texturedataindex;       // Offset to texture pixel data
    
    // Skins
    int                 numskinref;             // Number of replaceable textures
    int                 numskinfamilies;        // Number of skin families
    int                 skinindex;              // Offset to skin data
    
    // Body parts
    int                 numbodyparts;           // Number of bodypart data
    int                 bodypartindex;          // Offset to bodypart data
    
    // Attachments
    int                 numattachments;         // Number of queryable attachable points
    int                 attachmentindex;        // Offset to attachment data
    
    // Sounds
    int                 soundtable;
    int                 soundindex;
    int                 soundgroups;
    int                 soundgroupindex;
    
    // Transitions
    int                 numtransitions;         // Animation node to animation node transition graph
    int                 transitionindex;        // Offset to transition data
} studiohdr_t;

// ============================================================================
// BODY PARTS
// Groups of models (like different heads, torsos, etc.)
// ============================================================================
typedef struct
{
    char                name[64];               // Bodypart name
    int                 nummodels;              // Number of models in this bodypart
    int                 base;                   // Base value
    int                 modelindex;             // Offset to models array
} mstudiobodyparts_t;

// ============================================================================
// MODELS  
// Individual mesh within a bodypart
// ============================================================================
typedef struct
{
    char                name[64];               // Model name
    
    int                 type;                   // Model type
    
    float               boundingradius;         // Bounding sphere radius
    
    // Meshes
    int                 nummesh;                // Number of meshes
    int                 meshindex;              // Offset to first mesh
    
    // Vertices
    int                 numverts;               // Number of unique vertices/normals/texcoords
    int                 vertinfoindex;          // Offset to vertex bone info
    int                 vertindex;              // Offset to vertex vec3_t array
    
    // Normals  
    int                 numnorms;               // Number of unique surface normals
    int                 norminfoindex;          // Offset to normal bone info
    int                 normindex;              // Offset to normal vec3_t array
    
    // Groups
    int                 numgroups;              // Number of deformation groups
    int                 groupindex;             // Offset to group data
} mstudiomodel_t;

// ============================================================================
// MESHES
// Triangle mesh within a model
// ============================================================================
typedef struct
{
    int                 numtris;                // Number of triangles
    int                 triindex;               // Offset to triangle commands
    int                 skinref;                // Material/texture reference
    int                 numnorms;               // Per mesh normals
    int                 normindex;              // Offset to normal vec3_t array
} mstudiomesh_t;

// ============================================================================
// TRIANGLE VERTEX
// Individual vertex in triangle strip/fan command
// ============================================================================
typedef struct
{
    short               vertindex;             // Index into vertex array
    short               normalindex;           // Index into normal array
    short               s, t;                  // Texture coordinates (skin coords)
} mstudiotrivert_t;

// ============================================================================
// TEXTURES
// Material/texture information
// ============================================================================
typedef struct
{
    char                name[64];               // Texture name
    int                 flags;                  // Texture flags
    int                 width, height;          // Texture dimensions
    int                 index;                  // Offset into texture data
} mstudiotexture_t;

// ============================================================================
// BONES
// Skeletal bone structure
// ============================================================================
typedef struct
{
    char                name[32];               // Bone name for symbolic links
    int                 parent;                 // Parent bone index (-1 if root)
    int                 flags;                  // Bone flags
    int                 bonecontroller[6];      // Bone controller index, -1 = none
    float               value[6];               // Default DoF values
    float               scale[6];               // Scale for delta DoF values
} mstudiobone_t;

// ============================================================================
// BONE CONTROLLERS
// ============================================================================
typedef struct
{
    int                 bone;                   // -1 = 0
    int                 type;                   // X, Y, Z, XR, YR, ZR, M
    float               start;
    float               end;
    int                 rest;                   // byte index value at rest
    int                 index;                  // 0-3 user set controller, 4 mouth
} mstudiobonecontroller_t;

// ============================================================================
// HIT BOXES
// ============================================================================
typedef struct
{
    int                 bone;
    int                 group;                  // intersection group
    vec3_t              bbmin;                  // bounding box
    vec3_t              bbmax;
} mstudiobbox_t;

// ============================================================================
// ANIMATION SEQUENCES
// ============================================================================
typedef struct
{
    char                label[32];              // sequence label
    
    float               fps;                    // frames per second    
    int                 flags;                  // looping/non-looping flags
    
    int                 activity;
    int                 actweight;
    
    int                 numevents;
    int                 eventindex;
    
    int                 numframes;              // number of frames per sequence
    
    int                 numpivots;              // number of foot pivots
    int                 pivotindex;
    
    int                 motiontype;    
    int                 motionbone;
    vec3_t              linearmovement;
    int                 automoveposindex;
    int                 automoveangleindex;
    
    vec3_t              bbmin;                  // per sequence bounding box
    vec3_t              bbmax;
    
    int                 numblends;
    int                 animindex;              // mstudioanim_t pointer relative to start of sequence group data
                                                // [blend][bone][X, Y, Z, XR, YR, ZR]
    
    int                 blendtype[2];           // X, Y, Z, XR, YR, ZR
    float               blendstart[2];          // starting value
    float               blendend[2];            // ending value
    int                 blendparent;
    
    int                 seqgroup;               // sequence group for demand loading
    
    int                 entrynode;              // transition node at entry
    int                 exitnode;               // transition node at exit
    int                 nodeflags;              // transition rules
    
    int                 nextseq;                // auto advancing sequences
} mstudioseqdesc_t;

// ============================================================================
// EVENTS
// ============================================================================
typedef struct
{
    int                 frame;
    int                 event;
    int                 type;
    char                options[64];
} mstudioevent_t;

// ============================================================================
// PIVOTS
// ============================================================================
typedef struct
{
    vec3_t              org;                    // pivot point
    int                 start, end;
} mstudiopivot_t;

// ============================================================================
// ATTACHMENTS
// ============================================================================
typedef struct
{
    char                name[32];
    int                 type;
    int                 bone;
    vec3_t              org;                    // attachment point
    vec3_t              vectors[3];
} mstudioattachment_t;

// ============================================================================
// TRIANGLE COMMAND TYPES
// These are the values you'll see in triangle commands
// ============================================================================
#define STUDIO_RENDER_TRIANGLES     0
#define STUDIO_RENDER_TRIANGLE_STRIP 1
#define STUDIO_RENDER_TRIANGLE_FAN  2

// ============================================================================
// MODEL FLAGS
// ============================================================================
#define STUDIO_NF_FLATSHADE     0x0001
#define STUDIO_NF_CHROME        0x0002
#define STUDIO_NF_FULLBRIGHT    0x0004
#define STUDIO_NF_NOMIPS        0x0008
#define STUDIO_NF_ALPHA         0x0010
#define STUDIO_NF_ADDITIVE      0x0020
#define STUDIO_NF_MASKED        0x0040

// ============================================================================
// BONE FLAGS  
// ============================================================================
#define STUDIO_HAS_NORMALS      0x0001
#define STUDIO_HAS_VERTICES     0x0002
#define STUDIO_HAS_BBOX         0x0004
#define STUDIO_HAS_CHROME       0x0008

#ifdef __cplusplus
}
#endif

#endif // STUDIO_H
