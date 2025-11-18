// textured.vert
#version 410 core
  // Input attributes
  layout(location = 0) in vec3 aPos;        // Rest-pose position
  layout(location = 1) in vec3 aNormal;     // Rest-pose normal
  layout(location = 2) in vec2 aUV;
  layout(location = 3) in int aBoneIndex;   // NEW: Which bone controls this vertex

  // Uniforms
  uniform mat4 boneMatrices[128];  // NEW: Array of bone transforms
  uniform mat4 model;
  uniform mat4 view;
  uniform mat4 projection;
  
  // Adding new for chrome
  uniform bool u_chrome;
  uniform vec3 viewPos;

  // Outputs
  out vec3 vNormal;
  out vec3 vWorldPos;
  out vec2 vUV;

  void main() {
      // Transform vertex by its bone (in original Half-Life coordinates)
      vec4 skinnedPos = boneMatrices[aBoneIndex] * vec4(aPos, 1.0);
      vec4 skinnedNormal = boneMatrices[aBoneIndex] * vec4(aNormal, 0.0);

      // Apply viewer scale (0.1x)
      skinnedPos.xyz *= 0.1;

      // Apply axis remap (Half-Life → OpenGL coordinates)
      // Half-Life: X=right, Y=forward, Z=up
      // OpenGL: X=right, Y=up, Z=back
      // Transform: (x, y, z) → (x, z, -y)
      float tempY = skinnedPos.y;
      skinnedPos.y = skinnedPos.z;   // Z → Y (up)
      skinnedPos.z = -tempY;          // -Y → Z (back)

      // Same remap for normals
      tempY = skinnedNormal.y;
      skinnedNormal.y = skinnedNormal.z;
      skinnedNormal.z = -tempY;

      // Apply model/view/projection transforms
      vec4 world = model * skinnedPos;
      vWorldPos = world.xyz;
      vNormal = mat3(model) * skinnedNormal.xyz;
      
      // @Note: Adding here for chrome textures UVs
      if ( u_chrome ) {
          vec3 N = normalize(vNormal);

          // Half-Life chrome sphere mapping
          float m = 2.0 * sqrt(N.x*N.x + N.y*N.y + (N.z+1.0)*(N.z+1.0));
          vUV.x = N.x / m + 0.5;
          vUV.y = N.y / m + 0.5;      }
      else {
          vUV = aUV;
      }
      
      
      // vUV = aUV;
      gl_Position = projection * view * world;
  }
