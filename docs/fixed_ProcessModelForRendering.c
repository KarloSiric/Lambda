// Updated ProcessModelForRendering function with bodypart selection
// Replace your entire ProcessModelForRendering function in renderer.c with this:

void ProcessModelForRendering(void)
{
    if (!global_header || !global_data)
    {
        fprintf(stderr, "ERROR - Invalid argument pointers value passed!\n");
        return;
    }

    total_render_vertices = 0;
    g_num_ranges          = 0;

    mstudiobodyparts_t *bodyparts = (mstudiobodyparts_t *)(global_data + global_header->bodypartindex);

    // Build bones (pose) once before drawing
    SetUpBones(global_header, global_data);

    for (int bp = 0; bp < global_header->numbodyparts; ++bp)
    {
        mstudiobodyparts_t *bpRec  = &bodyparts[bp];
        mstudiomodel_t    *models = (mstudiomodel_t *)(global_data + bpRec->modelindex);

        // ===== KEY CHANGE: Only render the selected model for this bodypart =====
        int selected_model_index = bodypart_get_model_index(bp);
        
        // Skip if invalid index
        if (selected_model_index < 0 || selected_model_index >= bpRec->nummodels) {
            printf("Skipping bodypart %d: invalid model index %d\n", bp, selected_model_index);
            continue;
        }
        
        // Get ONLY the selected model (not all of them!)
        mstudiomodel_t *model = &models[selected_model_index];
        
        // Debug output (only print once or when model changes)
        if (!model_processed) {
            printf("Rendering bodypart %d '%s': model %d/%d '%s'\n", 
                   bp, bpRec->name, 
                   selected_model_index, bpRec->nummodels,
                   model->name);
        }
        // ===== END OF KEY CHANGE =====

        g_current.model        = model;
        g_current.vertices     = (vec3_t *)(global_data + model->vertindex);
        g_current.normals      = (vec3_t *)(global_data + model->normindex);
        g_current.vertex_count = model->numverts;
        g_current.normal_count = model->numnorms;

        // Skin this model's vertices (fills skinned_positions[])
        TransformVertices(global_header, global_data, model, skinned_positions);
        have_skinned_positions = true;

        // All meshes for this model
        mstudiomesh_t *meshes = (mstudiomesh_t *)(global_data + model->meshindex);

        // Skin table
        const short *skin_table  = (const short *)(global_data + global_header->skinindex);
        const int    numskinref  = global_header->numskinref;
        const int    skin_family = 0;

        for (int mesh = 0; mesh < model->nummesh; ++mesh)
        {
            const int norm_base = meshes[mesh].normindex;
            const int v_count   = g_current.vertex_count;
            const int n_count   = g_current.normal_count;

            // Resolve texture index via skin table
            int tex_index = meshes[mesh].skinref;
            if (skin_table && numskinref > 0 && tex_index >= 0 && tex_index < numskinref)
            {
                tex_index = skin_table[skin_family * numskinref + tex_index];
            }

            // GL texture + size
            GLuint gl_tex = 0;
            int    texW = 1, texH = 1;
            if (tex_index >= 0 && tex_index < g_textures.count)
            {
                gl_tex = g_textures.textures[tex_index].gl_id;
                texW = g_textures.textures[tex_index].width;
                texH = g_textures.textures[tex_index].height;

                if (texW <= 0) texW = 1;
                if (texH <= 0) texH = 1;
            }
            if (!gl_tex && g_white_tex)
            {
                gl_tex = g_white_tex;
                texW   = 2;
                texH   = 2;
            }

            printf("mesh %d: skinref %d -> tex %d  GL=%u  %dx%d\n",
                   mesh, meshes[mesh].skinref, tex_index, (unsigned)gl_tex, texW, texH);

            short *ptricmds = (short *)(global_data + meshes[mesh].triindex);

            const int start_first = total_render_vertices;

            int s_min =  32767, s_max = -32768;
            int t_min =  32767, t_max = -32768;
            int dbg_count = 0;

            int i;
            while ((i = *(ptricmds++)))
            {
                if (i < 0)
                {
                    // -------- triangle fan --------
                    i = -i;

                    // read first 2 vertices
                    short v0 = ptricmds[0];
                    short n0 = ptricmds[1];
                    short s0 = ptricmds[2];
                    short t0 = ptricmds[3];
                    ptricmds = (short *)((char *)ptricmds + 4 * sizeof(short));

                    short v1 = ptricmds[0];
                    short n1 = ptricmds[1];
                    short s1 = ptricmds[2];
                    short t1 = ptricmds[3];
                    ptricmds = (short *)((char *)ptricmds + 4 * sizeof(short));

                    // >>> FIX: apply ON-SEAM rule and clear high bit on normal index
                    if (n0 & 0x8000) { s0 = (short)(s0 + texW / 2); }
                    n0 &= 0x7FFF;
                    if (n1 & 0x8000) { s1 = (short)(s1 + texW / 2); }
                    n1 &= 0x7FFF;

                    // norm indices are relative to this mesh's block
                    n0 = (short)(n0 + norm_base);
                    n1 = (short)(n1 + norm_base);

                    for (int j = 2; j < i; ++j)
                    {
                        short v2 = ptricmds[0];
                        short n2 = ptricmds[1];
                        short s2 = ptricmds[2];
                        short t2 = ptricmds[3];
                        ptricmds = (short *)((char *)ptricmds + 4 * sizeof(short));

                        // >>> FIX: apply ON-SEAM rule and clear high bit
                        if (n2 & 0x8000) { s2 = (short)(s2 + texW / 2); }
                        n2 &= 0x7FFF;

                        // update min/max + quick sample print
                        if (dbg_count++ < 8) printf("  s=%d t=%d  (tex %dx%d)\n", s0, t0, texW, texH);
                        s_min = (s0 < s_min) ? s0 : s_min; s_max = (s0 > s_max) ? s0 : s_max;
                        t_min = (t0 < t_min) ? t0 : t_min; t_max = (t0 > t_max) ? t0 : t_max;

                        // normals block offset
                        n2 = (short)(n2 + norm_base);

                        // bounds guards
                        if (v0 >= 0 && v0 < v_count && n0 >= 0 && n0 < n_count &&
                            v1 >= 0 && v1 < v_count && n1 >= 0 && n1 < n_count &&
                            v2 >= 0 && v2 < v_count && n2 >= 0 && n2 < n_count)
                        {
                            AddVertexToBuffer(v0, n0, s0, t0, (float)texW, (float)texH);
                            AddVertexToBuffer(v1, n1, s1, t1, (float)texW, (float)texH);
                            AddVertexToBuffer(v2, n2, s2, t2, (float)texW, (float)texH);
                        }

                        // roll forward
                        v1 = v2; n1 = n2; s1 = s2; t1 = t2;
                    }
                }
                else
                {
                    // -------- triangle strip --------

                    // read first 2 vertices
                    short v0 = ptricmds[0];
                    short n0 = ptricmds[1];
                    short s0 = ptricmds[2];
                    short t0 = ptricmds[3];
                    ptricmds = (short *)((char *)ptricmds + 4 * sizeof(short));

                    short v1 = ptricmds[0];
                    short n1 = ptricmds[1];
                    short s1 = ptricmds[2];
                    short t1 = ptricmds[3];
                    ptricmds = (short *)((char *)ptricmds + 4 * sizeof(short));

                    // >>> FIX: seam + clear high bit
                    if (n0 & 0x8000) { s0 = (short)(s0 + texW / 2); }
                    n0 &= 0x7FFF;
                    if (n1 & 0x8000) { s1 = (short)(s1 + texW / 2); }
                    n1 &= 0x7FFF;

                    n0 = (short)(n0 + norm_base);
                    n1 = (short)(n1 + norm_base);

                    for (int j = 2; j < i; ++j)
                    {
                        short v2 = ptricmds[0];
                        short n2 = ptricmds[1];
                        short s2 = ptricmds[2];
                        short t2 = ptricmds[3];
                        ptricmds = (short *)((char *)ptricmds + 4 * sizeof(short));

                        // >>> FIX: seam + clear high bit
                        if (n2 & 0x8000) { s2 = (short)(s2 + texW / 2); }
                        n2 &= 0x7FFF;

                        if (dbg_count++ < 8) printf("  s=%d t=%d  (tex %dx%d)\n", s0, t0, texW, texH);
                        s_min = (s0 < s_min) ? s0 : s_min; s_max = (s0 > s_max) ? s0 : s_max;
                        t_min = (t0 < t_min) ? t0 : t_min; t_max = (t0 > t_max) ? t0 : t_max;

                        n2 = (short)(n2 + norm_base);

                        // two-triangle parity
                        if ((j - 2) % 2 == 0)
                        {
                            if (v0 >= 0 && v0 < v_count && n0 >= 0 && n0 < n_count &&
                                v1 >= 0 && v1 < v_count && n1 >= 0 && n1 < n_count &&
                                v2 >= 0 && v2 < v_count && n2 >= 0 && n2 < n_count)
                            {
                                AddVertexToBuffer(v0, n0, s0, t0, (float)texW, (float)texH);
                                AddVertexToBuffer(v1, n1, s1, t1, (float)texW, (float)texH);
                                AddVertexToBuffer(v2, n2, s2, t2, (float)texW, (float)texH);
                            }
                        }
                        else
                        {
                            if (v0 >= 0 && v0 < v_count && n0 >= 0 && n0 < n_count &&
                                v1 >= 0 && v1 < v_count && n1 >= 0 && n1 < n_count &&
                                v2 >= 0 && v2 < v_count && n2 >= 0 && n2 < n_count)
                            {
                                AddVertexToBuffer(v1, n1, s1, t1, (float)texW, (float)texH);
                                AddVertexToBuffer(v0, n0, s0, t0, (float)texW, (float)texH);
                                AddVertexToBuffer(v2, n2, s2, t2, (float)texW, (float)texH);
                            }
                        }

                        // roll forward
                        v0 = v1; n0 = n1; s0 = s1; t0 = t1;
                        v1 = v2; n1 = n2; s1 = s2; t1 = t2;
                    }
                }
            }

            // One draw range for this mesh
            if (g_num_ranges < MAX_DRAW_RANGES)
            {
                g_ranges[g_num_ranges].tex   = gl_tex;
                g_ranges[g_num_ranges].first = start_first;
                g_ranges[g_num_ranges].count = total_render_vertices - start_first;
                g_num_ranges++;
            }
        }

        printf("Generated %d vertices (%d triangles) for bodypart %d\n",
               total_render_vertices, total_render_vertices / 3, bp);
    }

    model_processed = true;
    
    printf("Total: %d vertices (%d triangles) across all bodyparts\n",
           total_render_vertices, total_render_vertices / 3);
}
