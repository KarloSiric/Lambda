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
	int complexity_score;                  // 0-100 scale
	const char *performance_rating;        // "Low", "Medium", "High", "Very High"

} mdl_stats_t;


void mdl_stats_analyze( const studiohdr_t *main_header,
                          const studiohdr_t *tex_header,
                          const unsigned char *main_data,
                          const unsigned char *tex_data,
                          mdl_stats_t *out_stats );

int mdl_stats_triangles_count( const studiohdr_t *header, const unsigned char *data );
int mdl_stats_count_vertices( const studiohdr_t *header, const unsigned char *data );
int mdl_stats_count_meshes( const studiohdr_t *header, const unsigned char *data );



#ifdef __cplusplus
}
#endif

#endif
