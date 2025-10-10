/***
*
*   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*   This product contains software technology licensed from Id
*   Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*   All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*   ==============================================================================
*
*   studio.h - Official Valve Half-Life Studio Model Format
*   Version 10 - Half-Life 1 / GoldSource Engine
*
*   Based on Valve's official Half-Life SDK headers (Pure C version)
*
*   ==============================================================================
*
****/

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
#define STUDIO_VERSION   10
#define STUDIO_VERSION_2 9
#define IDSTUDIOHEADER   ((('T' << 24) + ('S' << 16) + ('D' << 8) + 'I'))  // "IDST"
#define IDSEQGRPHEADER   ((('Q' << 24) + ('S' << 16) + ('D' << 8) + 'I'))  // "IDSQ"

// Model limits
#define MAXSTUDIOTRIANGLES   20000
#define MAXSTUDIOVERTS       2048
#define MAXSTUDIOSEQUENCES   2048
#define MAXSTUDIOSKINS       100
#define MAXSTUDIOSRCBONES    512
#define MAXSTUDIOBONES       128
#define MAXSTUDIOMODELS      32
#define MAXSTUDIOBODYPARTS   32
#define MAXSTUDIOGROUPS      16
#define MAXSTUDIOANIMATIONS  2048
#define MAXSTUDIOMESHES      256
#define MAXSTUDIOEVENTS      1024
#define MAXSTUDIOPIVOTS      256
#define MAXSTUDIOCONTROLLERS 8

// Type definitions
typedef float         vec3_t[3];
typedef float         vec4_t[4];
typedef unsigned char byte;

// ============================================================================
// MAIN STUDIO HEADER
// ============================================================================
typedef struct {
    int id;
    int version;

    char name[64];
    int  length;

    vec3_t eyeposition;
    vec3_t min;
    vec3_t max;
    vec3_t bbmin;
    vec3_t bbmax;

    int flags;

    int numbones;
    int boneindex;

    int numbonecontrollers;
    int bonecontrollerindex;

    int numhitboxes;
    int hitboxindex;

    int numseq;
    int seqindex;

    int numseqgroups;
    int seqgroupindex;

    int numtextures;
    int textureindex;
    int texturedataindex;

    int numskinref;
    int numskinfamilies;
    int skinindex;

    int numbodyparts;
    int bodypartindex;

    int numattachments;
    int attachmentindex;

    int soundtable;
    int soundindex;
    int soundgroups;
    int soundgroupindex;

    int numtransitions;
    int transitionindex;
} studiohdr_t;

// ============================================================================
// SEQUENCE GROUP HEADER
// ============================================================================
typedef struct {
    int  id;
    int  version;
    char name[64];
    int  length;
} studioseqhdr_t;

// ============================================================================
// BODY PARTS
// ============================================================================
typedef struct {
    char name[64];
    int  nummodels;
    int  base;
    int  modelindex;
} mstudiobodyparts_t;

// ============================================================================
// MODELS
// ============================================================================
typedef struct {
    char name[64];

    int type;

    float boundingradius;

    int nummesh;
    int meshindex;

    int numverts;
    int vertinfoindex;
    int vertindex;

    int numnorms;
    int norminfoindex;
    int normindex;

    int numgroups;
    int groupindex;
} mstudiomodel_t;

// ============================================================================
// MESHES
// ============================================================================
typedef struct {
    int numtris;
    int triindex;
    int skinref;
    int numnorms;
    int normindex;
} mstudiomesh_t;

// ============================================================================
// TRIANGLE VERTEX
// ============================================================================
typedef struct {
    short vertindex;
    short normalindex;
    short s, t;
} mstudiotrivert_t;

// ============================================================================
// TEXTURES
// ============================================================================
typedef struct {
    char name[64];
    int  flags;
    int  width, height;
    int  index;
} mstudiotexture_t;

// ============================================================================
// BONES
// ============================================================================
typedef struct {
    char  name[32];
    int   parent;
    int   flags;
    int   bonecontroller[6];
    float value[6];
    float scale[6];
} mstudiobone_t;

// ============================================================================
// BONE CONTROLLERS
// ============================================================================
typedef struct {
    int   bone;
    int   type;
    float start;
    float end;
    int   rest;
    int   index;
} mstudiobonecontroller_t;

// ============================================================================
// HIT BOXES
// ============================================================================
typedef struct {
    int    bone;
    int    group;
    vec3_t bbmin;
    vec3_t bbmax;
} mstudiobbox_t;

// ============================================================================
// ANIMATION SEQUENCES
// ============================================================================
typedef struct {
    char label[32];

    float fps;
    int   flags;

    int activity;
    int actweight;

    int numevents;
    int eventindex;

    int numframes;

    int numpivots;
    int pivotindex;

    int    motiontype;
    int    motionbone;
    vec3_t linearmovement;
    int    automoveposindex;
    int    automoveangleindex;

    vec3_t bbmin;
    vec3_t bbmax;

    int numblends;
    int animindex;

    int   blendtype[2];
    float blendstart[2];
    float blendend[2];
    int   blendparent;

    int seqgroup;

    int entrynode;
    int exitnode;
    int nodeflags;

    int nextseq;
} mstudioseqdesc_t;

// ============================================================================
// EVENTS
// ============================================================================
typedef struct {
    int  frame;
    int  event;
    int  type;
    char options[64];
} mstudioevent_t;

// ============================================================================
// ANIMATION DATA
// ============================================================================
typedef struct {
    unsigned short offset[6];
} mstudioanim_t;

typedef union {
    struct {
        byte valid;
        byte total;
    } num;
    short value;
} mstudioanimvalue_t;

// ============================================================================
// PIVOTS
// ============================================================================
typedef struct {
    vec3_t org;
    int    start, end;
} mstudiopivot_t;

// ============================================================================
// ATTACHMENTS
// ============================================================================
typedef struct {
    char   name[32];
    int    type;
    int    bone;
    vec3_t org;
    vec3_t vectors[3];
} mstudioattachment_t;

// ============================================================================
// SEQUENCE GROUPS
// ============================================================================
typedef struct {
    char label[32];
    char name[64];
    int  unused1;
    int  unused2;
} mstudioseqgroup_t;

// ============================================================================
// MODEL FLAGS
// ============================================================================
#define STUDIO_NF_FLATSHADE  0x0001
#define STUDIO_NF_CHROME     0x0002
#define STUDIO_NF_FULLBRIGHT 0x0004
#define STUDIO_NF_NOMIPS     0x0008
#define STUDIO_NF_ALPHA      0x0010
#define STUDIO_NF_ADDITIVE   0x0020
#define STUDIO_NF_MASKED     0x0040

// ============================================================================
// BONE FLAGS
// ============================================================================
#define STUDIO_HAS_NORMALS  0x0001
#define STUDIO_HAS_VERTICES 0x0002
#define STUDIO_HAS_BBOX     0x0004
#define STUDIO_HAS_CHROME   0x0008

// ============================================================================
// SEQUENCE FLAGS
// ============================================================================
#define STUDIO_LOOPING 0x0001

#ifdef __cplusplus
}
#endif

#endif  // STUDIO_H
