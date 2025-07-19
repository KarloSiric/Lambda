/*
* @Author: karlosiric
* @Date:   2025-07-18 12:28:34
* @Last Modified by:   karlosiric
* @Last Modified time: 2025-07-19 20:27:30
*/

#include "mdl_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_data(FILE *file, void *buffer, size_t size, const char *description) {
    size_t bytes_read = fread(buffer, 1, size, file);
    if (bytes_read != size) {
        fprintf(stderr, "Failed to read %s: expected %zu bytes and got %zu bytes\n", 
            description, size, bytes_read);
        return (0);
    }

    return (1);
}

void print_mdl_info(const char *filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Error opening file: '%s'\n", filepath);
        return;
    }

    printf("\nFILE opened successfully!\n\n");
    printf("==== MDL FILE INFORMATION ====\n");
    printf("FILE: '%s'\n", filepath);

    mdl_header_s mdl_header;

    printf("Printing position before reading: %ld\n", ftell(file));

    if (!read_data(file, &mdl_header.id, sizeof(mdl_header.id), "Magic ID")) {
        fclose(file);
        return;
    }

    printf("Printing position after ID: %ld\n", ftell(file));

    if (!read_data(file, &mdl_header.version, sizeof(mdl_header.version), "Version ID")) {
        fclose(file);
        return;
    }

    if (!read_data(file, mdl_header.name, sizeof(mdl_header.name), "Name")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.length, sizeof(mdl_header.length), "Data Length")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.eyeposition, sizeof(mdl_header.eyeposition), "Eye position")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.min, sizeof(mdl_header.min), "hull min")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.max, sizeof(mdl_header.max), "hull max")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.bbmin, sizeof(mdl_header.bbmin), "view bbmin")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.bbmax, sizeof(mdl_header.bbmax), "view bbmax")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.flags, sizeof(mdl_header.flags), "flags")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numbones, sizeof(mdl_header.numbones), "bone count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.boneindex, sizeof(mdl_header.boneindex), "bone offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numbonecontrollers, sizeof(mdl_header.numbonecontrollers), "bone controller count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.bonecontrollerindex, sizeof(mdl_header.bonecontrollerindex), "bone controller offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numhitboxes, sizeof(mdl_header.numhitboxes), "hitbox count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.hitboxindex, sizeof(mdl_header.hitboxindex), "hitbox offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numseq, sizeof(mdl_header.numseq), "seq count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.seqindex, sizeof(mdl_header.seqindex), "seq offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numseqgroups, sizeof(mdl_header.numseqgroups), "seqgroup count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.seqgroupindex, sizeof(mdl_header.seqgroupindex), "seqgroup offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numtextures, sizeof(mdl_header.numtextures), "texture count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.textureindex, sizeof(mdl_header.textureindex), "texture offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.texturedataindex, sizeof(mdl_header.texturedataindex), "texturedata offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numskinref, sizeof(mdl_header.numskinref), "skin ref count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numskinfamilies, sizeof(mdl_header.numskinfamilies), "skin families count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.skinindex, sizeof(mdl_header.skinindex), "skin offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numbodyparts, sizeof(mdl_header.numbodyparts), "bodypart count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.bodypartindex, sizeof(mdl_header.bodypartindex), "bodypart offset")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.numattachments, sizeof(mdl_header.numattachments), "attachment count")) {
        fclose(file);
        return;
    }

    if (!read_data(file, &mdl_header.attachmentindex, sizeof(mdl_header.attachmentindex), "attachment offset")) {
        fclose(file);
        return;
    }

    

    
    printf("Current position after complete header: %ld\n\n", ftell(file));

    printf("==== COMPLETE MDL HEADER INFORMATION ====\n");
    printf("MAGIC ID: %.4s\n", (char *)&mdl_header.id);
    printf("VERSION: %d\n", mdl_header.version);
    printf("Name: %s\n", mdl_header.name);
    printf("Data Length: %d bytes\n", mdl_header.length);
    printf("Eye Position: (%.2f, %.2f, %.2f)\n", 
           mdl_header.eyeposition.x, mdl_header.eyeposition.y, mdl_header.eyeposition.z);
    printf("Hull Min: (%.2f, %.2f, %.2f)\n", 
           mdl_header.min.x, mdl_header.min.y, mdl_header.min.z);
    printf("Hull Max: (%.2f, %.2f, %.2f)\n", 
           mdl_header.max.x, mdl_header.max.y, mdl_header.max.z);
    printf("BB Min: (%.2f, %.2f, %.2f)\n", 
           mdl_header.bbmin.x, mdl_header.bbmin.y, mdl_header.bbmin.z);
    printf("BB Max: (%.2f, %.2f, %.2f)\n", 
           mdl_header.bbmax.x, mdl_header.bbmax.y, mdl_header.bbmax.z);
    printf("Flags: 0x%08X\n", mdl_header.flags);
    printf("Bone Count: %d\n", mdl_header.numbones);
    printf("Bone Offset: %d\n", mdl_header.boneindex);
    
    printf("\n==== KEY VERTEX DATA LOCATIONS ====\n");
    printf("Bodypart Count: %d\n", mdl_header.numbodyparts);
    printf("Bodypart Offset: %d\n", mdl_header.bodypartindex);
    printf("Texture Count: %d\n", mdl_header.numtextures);
    printf("Sequence Count: %d\n", mdl_header.numseq);
    
    printf("\n==== NEXT STEP: FIND VERTICES! ====\n");
    printf("Now jumping to bodypart offset %d to find vertex data...\n", mdl_header.bodypartindex);

    fseek(file, mdl_header.bodypartindex, SEEK_SET);
    mdl_bodypart_s bodyparts[mdl_header.numbodyparts];
    for (int i = 0; i < mdl_header.numbodyparts; i++) {
        
        long next_bodypart_pos = mdl_header.bodypartindex + ((i + 1) * sizeof(mdl_bodypart_s));

        if (!read_data(file, &bodyparts[i], sizeof(mdl_bodypart_s), "bodypart")) {
            fprintf(stderr, "Cannot read bodypart[%d] at position %ld\n", i, ftell(file));
            return;
        }

        printf("\n==== EXPLORING ALL MODELS ====\n");
        printf("\n---- Bodypart[%d]: '%s' ----\n", i, bodyparts[i].name);
        printf("Bodypart[%d] -> name: '%s' \n", i, bodyparts[i].name);
        printf("Bodypart[%d] -> nummodels: %d \n", i, bodyparts[i].nummodels);
        printf("Bodypart[%d] -> base: %d \n", i, bodyparts[i].base);
        printf("Bodypart[%d] -> modelindex: %d \n", i, bodyparts[i].modelindex);

        for (int j = 0; j < bodyparts[i].nummodels; j++) {
            printf("\n---- MODEL[%d][%d] ----\n", i, j);

            int model_offset = bodyparts[i].modelindex + (j * sizeof(mdl_model_s));
            fseek(file, model_offset, SEEK_SET);
            mdl_model_s model;

            if (!read_data(file, &model, sizeof(mdl_model_s), "model")) {
                continue;
            }

            printf("  MODEL[%d][%d] name: '%s'  \n", i, j, model.name);
            printf("  MODEL[%d][%d] name: %d  \n", i, j, model.vertindex);
            printf("  MODEL[%d][%d] name: %d  \n", i, j, model.vertinfoindex);

            //TODO -> here we need to extract the vertices data that is necessary for our OpenGL
            if (model.numverts > 0) {
                fseek(file, model.vertindex, SEEK_SET);

                mdl_vertex_s vertices[model.numverts];
                fread(vertices, sizeof(mdl_vertex_s), model.numverts, file);

                printf("  Printing %d vertices for the MODEL[%d][%d]: '%s'  \n",
                        model.numverts, i, j, model.name);

                for (int k = 0; k < model.numverts; k++) {
                    printf("   Vertex[%d]: (x=%.2f, y=%.2f, z=%.2f2)\n", k,
                            vertices[k].x, vertices[k].y, vertices[k].z);
                }
                printf("\n");
            }
        }

        if (i < mdl_header.numbodyparts - 1) {
            fseek(file, next_bodypart_pos, SEEK_SET);
        }
    }

    fclose(file);

}

mdl_model_data_s load_mdl_file(const char *filepath) {
}
