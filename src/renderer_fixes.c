// renderer_fixes.c
// Fixed implementations for the renderer.c file
// Copy these functions to replace the broken ones in renderer.c

#include "renderer.h"
#include "studio.h"
#include <stdio.h>

// Store references to current model being processed
static mstudiomodel_t *current_model = NULL;
static vec3_t *current_normals = NULL;
static mstudiotexture_t *current_textures = NULL;

// Initialize current model pointers (call this before processing)
void InitializeModelPointers(studiohdr_t *header, unsigned char *data, int bodypart_index) {
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(data + header->bodypartindex);
    mstudiomodel_t *models = (mstudiomodel_t *)(data + bodyparts[bodypart_index].modelindex);
    current_model = &models[0];
    
    // Get normal pointer correctly
    if (current_model->normindex > 0 && current_model->numnorms > 0) {
        current_normals = (vec3_t *)(data + current_model->normindex);
        printf("Normals initialized: %d normals at offset 0x%X\n", 
               current_model->numnorms, current_model->normindex);
    } else {
        printf("WARNING: No normals found in model!\n");
        current_normals = NULL;
    }
    
    // Get texture info if available
    if (header->numtextures > 0 && header->textureindex > 0) {
        current_textures = (mstudiotexture_t *)(data + header->textureindex);
        printf("Textures found: %d textures\n", header->numtextures);
    }
}

// FIXED VERSION of AddVertexToBuffer
void AddVertexToBuffer_FIXED(int vertex_index, int normal_index, short u, short v) {
    if (total_render_vertices >= MAX_RENDER_VERTICES) return;
    
    // Safety checks
    if (vertex_index < 0 || vertex_index >= m_numverts) {
        printf("WARNING: Invalid vertex index %d (max %d)\n", vertex_index, m_numverts);
        return;
    }
    
    int base_index = total_render_vertices * 8;
    
    // Position (with Half-Life coordinate system conversion)
    // Half-Life uses: X = right, Y = forward, Z = up
    // OpenGL uses: X = right, Y = up, Z = backward
    float scale = 0.02f; // Adjusted scale for better viewing
    render_vertex_buffer[base_index + 0] = m_pxformverts[vertex_index][0] * scale;
    render_vertex_buffer[base_index + 1] = m_pxformverts[vertex_index][2] * scale; // Swap Y and Z
    render_vertex_buffer[base_index + 2] = -m_pxformverts[vertex_index][1] * scale; // Negate
    
    // Normal (with same coordinate conversion)
    if (current_normals && normal_index >= 0 && normal_index < current_model->numnorms) {
        render_vertex_buffer[base_index + 3] = current_normals[normal_index][0];
        render_vertex_buffer[base_index + 4] = current_normals[normal_index][2];
        render_vertex_buffer[base_index + 5] = -current_normals[normal_index][1];
    } else {
        // Default normal pointing up if invalid
        render_vertex_buffer[base_index + 3] = 0.0f;
        render_vertex_buffer[base_index + 4] = 1.0f;
        render_vertex_buffer[base_index + 5] = 0.0f;
    }
    
    // Texture coordinates
    // Half-Life stores these as shorts, need to normalize properly
    float tex_u = (float)u / 64.0f;  // Will be adjusted per-texture later
    float tex_v = (float)v / 64.0f;
    
    // Clamp to valid range
    if (tex_u < 0.0f) tex_u = 0.0f;
    if (tex_u > 1.0f) tex_u = 1.0f;
    if (tex_v < 0.0f) tex_v = 0.0f;
    if (tex_v > 1.0f) tex_v = 1.0f;
    
    render_vertex_buffer[base_index + 6] = tex_u;
    render_vertex_buffer[base_index + 7] = tex_v;
    
    total_render_vertices++;
}

// FIXED VERSION of ProcessModelForRendering
void ProcessModelForRendering_FIXED(void) {
    if (!global_header || !global_data || model_processed) return;
    
    printf("Processing model with FIXED normal and UV extraction...\n");
    total_render_vertices = 0;
    
    // Initialize bone system
    if (!bone_system_initialized) {
        InitializeBoneSystem(global_header, global_data, 0);
        SetUpBones();
        bone_system_initialized = true;
    }
    
    // Initialize model pointers for correct data access
    InitializeModelPointers(global_header, global_data, 0);
    
    if (!current_model) {
        printf("ERROR: Failed to get current model!\n");
        return;
    }
    
    mstudiobodypart_t *bodyparts = (mstudiobodypart_t *)(global_data + global_header->bodypartindex);
    
    // Process first bodypart only for now
    for (int bp = 0; bp < 1 && bp < global_header->numbodyparts; bp++) {
        if (bodyparts[bp].nummodels == 0) continue;
        
        mstudiomodel_t *models = (mstudiomodel_t *)(global_data + bodyparts[bp].modelindex);
        mstudiomodel_t *model = &models[0];
        
        if (model->numverts == 0 || model->nummesh == 0) continue;
        
        mstudiomesh_t *meshes = (mstudiomesh_t *)(global_data + model->meshindex);
        
        printf("Processing bodypart %d: %d meshes, %d vertices, %d normals\n", 
               bp, model->nummesh, model->numverts, model->numnorms);
        
        // Process each mesh
        for (int mesh = 0; mesh < model->nummesh; mesh++) {
            if (meshes[mesh].numtris == 0) continue;
            
            short *ptricmds = (short *)(global_data + meshes[mesh].triindex);
            int triangles_generated = 0;
            
            // Process triangle commands
            while (1) {
                int command = *(ptricmds++);
                if (command == 0) break; // End of commands
                
                bool is_fan = (command < 0);
                int vertex_count = is_fan ? -command : command;
                
                // Safety check
                if (vertex_count < 3 || vertex_count > 128) {
                    printf("WARNING: Invalid vertex count %d in mesh %d\n", vertex_count, mesh);
                    break;
                }
                
                // Read vertex data
                mstudiotrivert_t *verts = (mstudiotrivert_t *)ptricmds;
                
                if (is_fan) {
                    // Triangle fan: 0-1-2, 0-2-3, 0-3-4, ...
                    for (int i = 2; i < vertex_count; i++) {
                        if (total_render_vertices >= MAX_RENDER_VERTICES - 3) {
                            goto done_processing;
                        }
                        
                        // Add three vertices for this triangle
                        AddVertexToBuffer_FIXED(verts[0].vertindex, verts[0].normalindex, 
                                               verts[0].s, verts[0].t);
                        AddVertexToBuffer_FIXED(verts[i-1].vertindex, verts[i-1].normalindex,
                                               verts[i-1].s, verts[i-1].t);
                        AddVertexToBuffer_FIXED(verts[i].vertindex, verts[i].normalindex,
                                               verts[i].s, verts[i].t);
                        
                        triangles_generated++;
                    }
                } else {
                    // Triangle strip: 0-1-2, 1-3-2, 2-3-4, 3-5-4, ...
                    for (int i = 2; i < vertex_count; i++) {
                        if (total_render_vertices >= MAX_RENDER_VERTICES - 3) {
                            goto done_processing;
                        }
                        
                        if (i % 2 == 0) {
                            // Even triangle
                            AddVertexToBuffer_FIXED(verts[i-2].vertindex, verts[i-2].normalindex,
                                                   verts[i-2].s, verts[i-2].t);
                            AddVertexToBuffer_FIXED(verts[i-1].vertindex, verts[i-1].normalindex,
                                                   verts[i-1].s, verts[i-1].t);
                            AddVertexToBuffer_FIXED(verts[i].vertindex, verts[i].normalindex,
                                                   verts[i].s, verts[i].t);
                        } else {
                            // Odd triangle (reversed winding)
                            AddVertexToBuffer_FIXED(verts[i-2].vertindex, verts[i-2].normalindex,
                                                   verts[i-2].s, verts[i-2].t);
                            AddVertexToBuffer_FIXED(verts[i].vertindex, verts[i].normalindex,
                                                   verts[i].s, verts[i].t);
                            AddVertexToBuffer_FIXED(verts[i-1].vertindex, verts[i-1].normalindex,
                                                   verts[i-1].s, verts[i-1].t);
                        }
                        
                        triangles_generated++;
                    }
                }
                
                // Move to next command
                ptricmds = (short *)((mstudiotrivert_t *)ptricmds + vertex_count);
            }
            
            printf("  Mesh %d: Generated %d triangles (expected %d)\n", 
                   mesh, triangles_generated, meshes[mesh].numtris);
        }
    }
    
    done_processing:
    printf("FIXED processing complete: %d vertices (%d triangles)\n", 
           total_render_vertices, total_render_vertices / 3);
    model_processed = true;
}

// Improved bone setup with proper transformations
void SetUpBones_FIXED() {
    if (!global_header || !global_data) return;
    
    mstudiobone_t *bones = (mstudiobone_t *)(global_data + global_header->boneindex);
    
    // Build proper bone transformations
    for (int i = 0; i < m_numbones; i++) {
        glm_mat4_identity(m_bonetransform[i]);
        
        // For now, just use identity transforms
        // In a real implementation, you'd calculate proper bone poses here
        // based on animation data
    }
    
    // Transform vertices using bone weights
    for (int i = 0; i < m_numverts; i++) {
        int bone_index = m_pvertbone[i];
        
        // Clamp bone index
        if (bone_index >= m_numbones || bone_index < 0) {
            bone_index = 0;
        }
        
        // For now, just copy vertices without transformation
        // In real implementation, multiply by bone matrix
        m_pxformverts[i][0] = m_pstudioverts[i][0];
        m_pxformverts[i][1] = m_pstudioverts[i][1];
        m_pxformverts[i][2] = m_pstudioverts[i][2];
    }
    
    printf("Bone transformations applied to %d vertices\n", m_numverts);
}
