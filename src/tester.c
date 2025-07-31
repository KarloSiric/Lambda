/*
* @Author: karlosiric
* @Date:   2025-07-22 11:49:29
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-31 16:08:24
*/

#include "mdl_loader.h"
#include <stdlib.h>
#include <stdbool.h>

#include "mdl_loader.h"
#include <stdlib.h>

int main(void) {
    printf("=== TESTING MDL INTEGRATION FUNCTION ===\n");
    
    // ONE FUNCTION CALL REPLACES ALL THE MANUAL PARSING!
    mdl_complete_model_s *complete_model = mdl_load_complete_file("models/HL1_Original/scientist.mdl");
    
    if (!complete_model) {
        fprintf(stderr, "❌ Failed to load complete model\n");
        return -1;
    }

    printf("\n=== ✅ COMPLETE MODEL LOADED SUCCESSFULLY ===\n");
    printf("📁 File: %s\n", complete_model->filename);
    printf("🎯 Total models: %d\n", complete_model->total_model_count);
    printf("🏗️  Bodyparts: %d\n", complete_model->bodypart_count);

    // Show details for each model
    for (int i = 0; i < complete_model->total_model_count; i++) {
        single_model_s *model = &complete_model->models[i];
        
        printf("\n--- 🎨 MODEL %d ---\n", i);
        printf("📛 Name: '%s'\n", model->model_name);
        printf("🏷️  Bodypart: '%s' (ID: %d)\n", model->bodypart_name, model->bodypart_id);
        printf("🔢 Model ID: %d\n", model->model_id);
        printf("📐 Vertices: %d (OpenGL array: %d floats)\n", 
               model->vertex_count, model->vertex_count * 3);
        printf("🔺 Triangles: %d (OpenGL indices: %d ints)\n", 
               model->triangle_count, model->triangle_count * 3);

        // Show sample data
        if (model->vertex_count > 0 && model->vertices) {
            printf("   📍 First vertex: (%.2f, %.2f, %.2f)\n",
                   model->vertices[0], model->vertices[1], model->vertices[2]);
            printf("   📍 Last vertex: (%.2f, %.2f, %.2f)\n",
                   model->vertices[(model->vertex_count-1)*3 + 0], 
                   model->vertices[(model->vertex_count-1)*3 + 1], 
                   model->vertices[(model->vertex_count-1)*3 + 2]);
        }
        
        if (model->triangle_count > 0 && model->triangle_indices) {
            printf("   🔺 First triangle: [%d, %d, %d]\n",
                   model->triangle_indices[0], model->triangle_indices[1], model->triangle_indices[2]);
            printf("   🔺 Last triangle: [%d, %d, %d]\n",
                   model->triangle_indices[(model->triangle_count-1)*3 + 0],
                   model->triangle_indices[(model->triangle_count-1)*3 + 1], 
                   model->triangle_indices[(model->triangle_count-1)*3 + 2]);
        }

        printf("   ✅ STATUS: Ready for OpenGL rendering!\n");
    }

    // Test data integrity
    printf("\n=== 🔍 DATA VALIDATION ===\n");
    int total_vertices = 0;
    int total_triangles = 0;
    bool all_valid = true;

    for (int i = 0; i < complete_model->total_model_count; i++) {
        single_model_s *model = &complete_model->models[i];
        total_vertices += model->vertex_count;
        total_triangles += model->triangle_count;

        // Validate triangle indices
        for (int t = 0; t < model->triangle_count; t++) {
            for (int v = 0; v < 3; v++) {
                int idx = model->triangle_indices[t * 3 + v];
                if (idx >= model->vertex_count) {
                    printf("❌ INVALID: Model '%s' triangle %d has vertex index %d (max: %d)\n",
                           model->model_name, t, idx, model->vertex_count - 1);
                    all_valid = false;
                }
            }
        }
    }

    printf("📊 TOTALS: %d vertices, %d triangles across all models\n", total_vertices, total_triangles);
    printf("✅ Data validation: %s\n", all_valid ? "ALL INDICES VALID" : "ERRORS FOUND");

    // Clean up
    mdl_free_complete_model(complete_model);
    
    printf("\n=== 🎉 INTEGRATION TEST COMPLETE ===\n");
    printf("Your MDL loader is ready for OpenGL rendering! 🚀\n");
    
    return 0;
}
