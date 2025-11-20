/*
 * ═══════════════════════════════════════════════════════════════════════════
 *   Half-Life Model Viewer/Editor ~ Lambda
 * ═══════════════════════════════════════════════════════════════════════════
 *
 *   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 *   This product contains software technology licensed from Id
 *   Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 *   All Rights Reserved.
 *
 *   Use, distribution, and modification of this source code and/or resulting
 *   object code is restricted to non-commercial enhancements to products from
 *   Valve LLC. All other use, distribution, or modification is prohibited
 *   without written permission from Valve LLC.
 *
 * ───────────────────────────────────────────────────────────────────────────
 *   Author: Karlo Siric
 *   Purpose: Statistics analysis file for outputting performance (mini profiler)
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "mdl_stats.h"
#include <math_types.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>

int mdl_stats_count_triangles( const studiohdr_t *header, const unsigned char *data ) {
	if ( header == NULL || data == NULL ) {
		return ( -1 );
	}

	int triangle_count = 0;

	int numbodyparts = header->numbodyparts;
	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( data + header->bodypartindex );

	for ( int b_idx = 0; b_idx < numbodyparts; b_idx++ ) {
		mstudiobodyparts_t *bodypart = &bodyparts[b_idx];

		mstudiomodel_t *models = (mstudiomodel_t *)( data + bodypart->modelindex );

		for ( int m = 0; m < bodypart->nummodels; m++ ) {
			mstudiomodel_t *model = &models[m];
			mstudiomesh_t *meshes = (mstudiomesh_t *)( data + model->meshindex );

			// @Note: Now we count the amount of triangles inside each of the meshes
			for ( int mesh_idx = 0; mesh_idx < model->nummesh; mesh_idx++ ) {
				mstudiomesh_t *mesh = &meshes[mesh_idx];
				short *ptricmds = (short *)( data + mesh->triindex );

				int i;
				while ( ( i = *( ptricmds++ ) ) != 0 ) {
					int vertex_count = ( i < 0 ) ? -i : i;

					// @Note: Number of triangles ir always using the formula (vertex_count - 2)
					//        Regardless if it's strips or fans ( > 0) or ( < 0);

					// @Safety Check: Adding safety check if vertex_count is bigger than 3
					//                Need atleast 3 vertices to make a triangle
					if ( vertex_count >= 3 ) {
						triangle_count += ( vertex_count - 2 );
					}
					ptricmds += vertex_count * 4;
				}
			}
		}
	}

	return triangle_count;
}

int mdl_stats_count_vertices( const studiohdr_t *header, const unsigned char *data ) {
	if ( header == NULL || data == NULL ) {
		return ( -1 );
	}

	int vertex_count = 0;

	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( data + header->bodypartindex );
	int numbodyparts = header->numbodyparts;
	for ( int bp_idx = 0; bp_idx < numbodyparts; bp_idx++ ) {
		mstudiobodyparts_t *bodypart = &bodyparts[bp_idx];
		int nummodels = bodypart->nummodels;

		mstudiomodel_t *models = (mstudiomodel_t *)( data + bodypart->modelindex );
		for ( int m_idx = 0; m_idx < nummodels; m_idx++ ) {
			mstudiomodel_t *model = &models[m_idx];

			// @Note: Now we add the count of the vertices to the total count
			vertex_count += model->numverts;
		}
	}

	return ( vertex_count );
}

int mdl_stats_count_meshes( const studiohdr_t *header, const unsigned char *data ) {
	if ( header == NULL || data == NULL ) {
		return ( -1 );
	}

	int mesh_count = 0;

	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( data + header->bodypartindex );
	int numbodyparts = header->numbodyparts;

	for ( int bp_idx = 0; bp_idx < numbodyparts; bp_idx++ ) {
		mstudiobodyparts_t *bodypart = &bodyparts[bp_idx];
		int nummodels = bodypart->nummodels;

		mstudiomodel_t *models = (mstudiomodel_t *)( data + bodypart->modelindex );

		for ( int m_idx = 0; m_idx < nummodels; m_idx++ ) {
			mstudiomodel_t *model = &models[m_idx];
			mesh_count += model->nummesh;
		}
	}

	return ( mesh_count );
}

int mdl_stats_count_normals( const studiohdr_t *header, const unsigned char *data ) {
	if ( header == NULL || data == NULL ) {
		return ( -1 );
	}

	int normals_count = 0;

	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( data + header->bodypartindex );
	int numbodyparts = header->numbodyparts;

	for ( int bp_idx = 0; bp_idx < numbodyparts; bp_idx++ ) {
		mstudiobodyparts_t *bodypart = &bodyparts[bp_idx];
		int nummodels = bodypart->nummodels;

		mstudiomodel_t *models = (mstudiomodel_t *)( data + bodypart->modelindex );

		for ( int m_idx = 0; m_idx < nummodels; m_idx++ ) {
			mstudiomodel_t *model = &models[m_idx];
			normals_count += model->numnorms;
		}
	}

	return ( normals_count );
}

int mdl_stats_count_groups( const studiohdr_t *header, const unsigned char *data ) {
	if ( header == NULL || data == NULL ) {
		return ( -1 );
	}

	int groups_count = 0;

	mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)( data + header->bodypartindex );
	int numbodyparts = header->numbodyparts;

	for ( int bp_idx = 0; bp_idx < numbodyparts; bp_idx++ ) {
		mstudiobodyparts_t *bodypart = &bodyparts[bp_idx];
		int nummodels = bodypart->nummodels;

		mstudiomodel_t *models = (mstudiomodel_t *)( data + bodypart->modelindex );

		for ( int m_idx = 0; m_idx < nummodels; m_idx++ ) {
			mstudiomodel_t *model = &models[m_idx];
			groups_count += model->numgroups;
		}
	}

	return ( groups_count );
}

size_t mdl_stats_texture_memory( const studiohdr_t *tex_header, const unsigned char *tex_data ) {
	if ( tex_header == NULL || tex_data == NULL ) {
		// @Note(Karlo): The return value is in size_t returning bytes so this needs to be 0 for that.
		return ( 0 );
	}

	// @Note: Safety check for if we do not have any textures whatsoever
	if ( tex_header->numtextures <= 0 ) {
		return ( 0 );
	}

	int numtextures = tex_header->numtextures;
	size_t total_bytes_size = 0;

	// @Note: We use the textureindex and not the texturedataindex! Do not mix them
	mstudiotexture_t *textures = (mstudiotexture_t *)( tex_data + tex_header->textureindex );
	for ( int i = 0; i < numtextures; i++ ) {
		mstudiotexture_t *tex = &textures[i];

		// @Brief: we now find the memory accumulated for this particular texture
		size_t tex_size = ( (size_t)tex->width * (size_t)tex->height ) * 4;

		total_bytes_size += tex_size;
	}

	return ( total_bytes_size );
}

void mdl_stats_texture_resolutions( const studiohdr_t *tex_header, const unsigned char *tex_data, int *min_res, int *max_res, int *avg_res ) {
	if ( tex_header == NULL || tex_data == NULL || min_res == NULL || max_res == NULL || avg_res == NULL ) {
		return;
	}

	*min_res = 0;
	*max_res = 0;
	*avg_res = 0;

	// Local variables for tracking the min and max and also avg values
	int min_area = INT_MAX;
	int max_area = 0;
	int avg_area = 0;

	int total_area = 0;

	// @Note: Safety check for if we do not have any textures whatsoever
	if ( tex_header->numtextures <= 0 ) {
		return;
	}

	int numtextures = tex_header->numtextures;
	mstudiotexture_t *textures = (mstudiotexture_t *)( tex_data + tex_header->textureindex );

	for ( int t_idx = 0; t_idx < numtextures; t_idx++ ) {
		mstudiotexture_t *tex = &textures[t_idx];

		int area = tex->width * tex->height;

		if ( area < min_area ) {
			min_area = area;
		}

		if ( area > max_area ) {
			max_area = area;
		}

		total_area += area;
	}

	avg_area = total_area / numtextures;

	// @Note: FInally we write it to the output pointers dereferenced
	*min_res = min_area;
	*max_res = max_area;
	*avg_res = avg_area;

	return;
}

int mdl_stats_count_root_bones( const studiohdr_t *header, const unsigned char *data ) {
	if ( header == NULL || data == NULL ) {
		return ( -1 );
	}

	// @Note: Safety checking is always good make sure to do it!
	if ( header->numbones <= 0 ) {
		return ( -1 );
	}

	int root_bones = 0;
	int numbones = header->numbones;
	mstudiobone_t *bones = (mstudiobone_t *)( data + header->boneindex );

	for ( int b_idx = 0; b_idx < numbones; b_idx++ ) {
		mstudiobone_t *bone = &bones[b_idx];

		// @Note: -1 in this case indicates that it is a root bone
		if ( bone->parent == -1 ) {
			root_bones++;
		}
	}

	return ( root_bones );
}

int mdl_stats_count_leaf_bones( const studiohdr_t *header, const unsigned char *data ) {
	if ( header == NULL || data == NULL ) {
		return ( -1 );
	}

	if ( header->numbones <= 0 ) {
		return ( -1 );
	}

	// @Note: Those bones that do not have any children because their children to them they are parents
	int leaf_bones = 0;
	int numbones = header->numbones;

	mstudiobone_t *bones = (mstudiobone_t *)( data + header->boneindex );

	for ( int i = 0; i < numbones; i++ ) {
		bool has_children = false;

		for ( int j = 0; j < numbones; j++ ) {
			if ( bones[j].parent == i ) {
				has_children = true;
				break;
			}
		}

		if ( !has_children ) {
			leaf_bones++;
		}
	}

	return ( leaf_bones );
}

int mdl_stats_bone_hierarchy_depth( const studiohdr_t *header, const unsigned char *data ) {
	if ( header == NULL || data == NULL ) {
		return ( -1 );
	}

	if ( header->numbones <= 0 ) {
		return ( -1 );
	}

	int max_depth = 0;

	int numbones = header->numbones;

	mstudiobone_t *bones = (mstudiobone_t *)( data + header->boneindex );

	for ( int i = 0; i < numbones; i++ ) {
		int depth = 0;
		int current = i;

		while ( bones[current].parent != -1 ) {
			depth++;
			current = bones[current].parent;

			if ( depth > numbones ) {
				break;
			}
		}

		if ( depth > max_depth ) {
			max_depth = depth;
		}
	}

	return ( max_depth );
}

float mdl_stats_total_animation_time( const studiohdr_t *header, const unsigned char *data ) {
	if ( header == NULL || data == NULL ) {
		return ( 0.0f );
	}

	if ( header->numseq <= 0 ) {
		return ( 0.0f );
	}

	float total_time = 0.0f;

	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( data + header->seqindex );

	int numsequences = header->numseq;

	for ( int i = 0; i < numsequences; i++ ) {
		mstudioseqdesc_t *sequence = &sequences[i];
		if ( sequence->fps > 0.0f ) {
			float duration = (float)sequence->numframes / sequence->fps;
			total_time += duration;
		}
	}

	return ( total_time );
}

int mdl_stats_total_frames( const studiohdr_t *header, const unsigned char *data ) {
	if ( header == NULL || data == NULL ) {
		return ( -1 );
	}
    
    if ( header->numseq <= 0 ) {
        return ( 0 );
    }


	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( data + header->seqindex );
	int numseq = header->numseq;
	int total_frames = 0;

	for ( int i = 0; i < numseq; i++ ) {
		mstudioseqdesc_t *sequence = &sequences[i];

		total_frames += sequence->numframes;
	}

	return ( total_frames );
}

int mdl_stats_count_events( const studiohdr_t *header, const unsigned char *data ) {    
	if ( header == NULL || data == NULL ) {
		return ( -1 );
	}
    
    if ( header->numseq <= 0 ) {
        return ( 0 );
    }
 
	int total_events = 0;
	int numseq = header->numseq;

	mstudioseqdesc_t *sequences = (mstudioseqdesc_t *)( data + header->seqindex );

	for ( int i = 0; i < numseq; i++ ) {
		mstudioseqdesc_t *sequence = &sequences[i];

		total_events += sequence->numevents;
	}

	return ( total_events );
}


void mdl_stats_get_dimensions( const studiohdr_t *header, float *width, float *height, float *depth ) {
    
    if ( header == NULL || width == NULL || height == NULL || depth == NULL ) {
        return ;        
    }
    
    // @Note: Now we calculate the dimensions for each model
    *width  = header->max[0] - header->min[0];
    *height = header->max[1] - header->min[1];
    *depth  = header->max[2] - header->min[2]; 
}


float mdl_stats_get_volume( const studiohdr_t *header ) {
    
    if ( header == NULL ) {
        return ( 0.0f );
    }
    
    float width = header->max[0] - header->min[0];
    float height = header->max[1] - header->min[1];
    float depth = header->max[2] - header->min[2];
    
    return width * height * depth;
}


size_t mdl_stats_estimate_memory( const studiohdr_t *main_header, const studiohdr_t *tex_header,
                                  const unsigned char *main_data, const unsigned char *tex_data ) {
  
    if ( main_header == NULL || tex_header == NULL || main_data == NULL || tex_data == NULL ) {
        return ( 0 );
    }
    
    size_t total_bytes = 0;
    
    total_bytes += mdl_stats_texture_memory( tex_header, tex_data );
    
    // Note: times 36 because we have from shader: 3 floats for position -> 12 bytes,
    //       normal is 3 floats so 12 bytes, UV is 2 floats so 8 bytes and bone is 4 bytes
    if ( main_header && main_data ) {
        int num_verts = mdl_stats_count_vertices( main_header, main_data );
        total_bytes += num_verts * 36;
    }
    
    // @Note: times 64 bytes because matrix 4 x 4 has 16 floats which is 64 bytes of size
    if ( main_header ) {
        total_bytes += main_header->numbones * 64;
    }
    
    // @Note: 28 bytes because position has 3 floats so 12 bytes and rotation has 4 floats so 16 bytes
    if ( main_header && main_data ) {
        int total_frames = mdl_stats_total_frames( main_header, main_data );
        int num_bones = main_header->numbones;
        total_bytes += total_frames * num_bones * 28; 
    }
    
    return ( total_bytes );
}


void mdl_stats_analyze( const studiohdr_t *main_header, const studiohdr_t *tex_header, const unsigned char *main_data, const unsigned char *tex_data, mdl_stats_t *out_stats ) {
    
    // Safety checking
    if ( main_header == NULL || tex_header == NULL || main_data == NULL || tex_data == NULL || out_stats == NULL ) {
        return ;
    }
    
    memset( out_stats, 0, sizeof( mdl_stats_t ) );
    
    out_stats->triangles = mdl_stats_count_triangles( main_header, main_data );
    out_stats->vertices = mdl_stats_count_vertices( main_header, main_data );
    out_stats->meshes = mdl_stats_count_meshes( main_header, main_data );
     
    if ( tex_header ) {
        out_stats->texture_count = tex_header->numtextures;
        out_stats->texture_memory_bytes = mdl_stats_texture_memory( tex_header, tex_data );        
        mdl_stats_texture_resolutions( tex_header, tex_data, &out_stats->min_texture_resolution, &out_stats->max_texture_resolution, &out_stats->avg_texture_resolution );        
    }
    
    if ( main_header ) {
        out_stats->bone_count = main_header->numbones;
        out_stats->root_bones = mdl_stats_count_root_bones( main_header, main_data );
        out_stats->hierarchy_depth = mdl_stats_bone_hierarchy_depth( main_header, main_data );
        out_stats->leaf_bones = mdl_stats_count_leaf_bones( main_header, main_data ); 
    }
    
    
    if ( main_header ) {
        out_stats->sequence_count = main_header->numseq;
        out_stats->total_frames = mdl_stats_total_frames( main_header, main_data );
        out_stats->total_animation_time = mdl_stats_total_animation_time( main_header, main_data );
        out_stats->total_events = mdl_stats_count_events( main_header, main_data );
    }
    
    if ( main_header ) {
        out_stats->attachment_count = main_header->numattachments;
        out_stats->hitbox_count = main_header->numhitboxes;
    }
    
    if ( main_header ) {
        mdl_stats_get_dimensions( main_header, 
                                  &out_stats->width,
                                  &out_stats->height,
                                  &out_stats->depth );
        out_stats->volume = mdl_stats_get_volume( main_header );
    }
     
    // Memory calculations
    out_stats->file_size_bytes = 0;
    out_stats->estimated_ram_bytes = mdl_stats_estimate_memory( main_header, tex_header, main_data, tex_data );
    out_stats->estimated_vram_bytes = mdl_stats_texture_memory( tex_header, tex_data ); 
}




