#ifndef STATS_H
#define STATS_H

#include "studio.h"
#include <stdio.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	// Geometry
	int triangles;
	int vertices;
	int meshes;

	// Textures
	int texture_count;
	size_t texture_memory_bytes;
	int min_texture_resolution;
	int max_texture_resolution;
	int avg_texture_resolution;

	// Bones
	int bone_count;
	int root_bones;
	int hierarchy_depth;
	int leaf_bones;

	// Animations
	int sequence_count;
	int total_frames;
	float total_animation_time;
	int total_events;

	// Attachments & Hitboxes
	int attachment_count;
	int hitbox_count;

	// Model properties
	float width;
	float height;
	float depth;
	float volume;

	// Memory
	size_t file_size_bytes;
	size_t estimated_ram_bytes;
	size_t estimated_vram_bytes;

	// Performance rating
	int complexity_score; // 0-100 scale
	const char *performance_rating; // "Low", "Medium", "High", "Very High"

} mdl_stats_t;

void mdl_stats_analyze( const studiohdr_t *main_header,
						const studiohdr_t *tex_header,
						const unsigned char *main_data,
						const unsigned char *tex_data,
						mdl_stats_t *out_stats );

/* Vertices and part of the geometry counting */
int mdl_stats_count_triangles( const studiohdr_t *header, const unsigned char *data );
int mdl_stats_count_vertices( const studiohdr_t *header, const unsigned char *data );
int mdl_stats_count_meshes( const studiohdr_t *header, const unsigned char *data );
int mdl_stats_count_normals( const studiohdr_t *header, const unsigned char *data );
int mdl_stats_count_groups( const studiohdr_t *header, const unsigned char *data );


/* Part of the Texture Statistics */
size_t mdl_stats_texture_memory( const studiohdr_t *tex_header, const unsigned char *tex_data );
void mdl_stats_texture_resolutions( const studiohdr_t *tex_header, const unsigned char *tex_data,
									int *min_res, int *max_res, int *avg_res );

/* Part of the Bones statistics */
int mdl_stats_bone_hierarchy_depth( const studiohdr_t *header, const unsigned char *data );
int mdl_stats_count_root_bones( const studiohdr_t *header, const unsigned char *data );
int mdl_stats_count_leaf_bones( const studiohdr_t *header, const unsigned char *data );

/* Part of the Animations statistics */
float mdl_stats_total_animation_time( const studiohdr_t *header, const unsigned char *data );
int mdl_stats_total_frames( const studiohdr_t *header, const unsigned char *data );
int mdl_stats_count_events( const studiohdr_t *header, const unsigned char *data );

/* Part of the model statistics */
void mdl_stats_get_dimensions( const studiohdr_t *header, float *width, float *height, float *depth );
float mdl_stats_get_volume( const studiohdr_t *header );

/* Part of the Memory statistics */
size_t mdl_stats_estimate_memory( const studiohdr_t *main_header, const studiohdr_t *tex_header,
								  const unsigned char *main_data, const unsigned char *tex_data );

void mdl_stats_print_report( FILE *output, const mdl_stats_t *stats, const char *model_name );

void mdl_stats_print_summary( FILE *output, const mdl_stats_t *stats );

void mdl_stats_print_performance( FILE *output, const mdl_stats_t *stats );

#ifdef __cplusplus
}
#endif

#endif
