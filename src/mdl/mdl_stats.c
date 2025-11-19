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
    
    
    
    
}
