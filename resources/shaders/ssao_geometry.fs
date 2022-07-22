#version 410
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform vec4 diffuse_color;

uniform bool sampleTexture = false;

void main()
{    
    
    gPosition = FragPos;
    
    //normal mapping
    // gNormal = texture(texture_normal1, TexCoords).rgb;
    // gNormal = normalize(gNormal * 2.0 - 1.0);

    gNormal = normalize(Normal);
    //albedo mapping
    vec4 color_tex = texture(texture_diffuse1, TexCoords);

    if(sampleTexture){
        gAlbedo = diffuse_color * color_tex;
    }
    else{
        gAlbedo = diffuse_color;
    }
    
}