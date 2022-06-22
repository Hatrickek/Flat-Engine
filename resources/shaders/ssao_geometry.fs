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

uniform vec4 color;

void main()
{    
    
    gPosition = FragPos;
    
    //normal mapping
    // gNormal = texture(texture_normal1, TexCoords).rgb;
    // gNormal = normalize(gNormal * 2.0 - 1.0);

    gNormal = normalize(Normal);
    //albedo mapping
    vec4 color_tex = texture(texture_diffuse1, TexCoords);
    if(color != vec4(0)){
        gAlbedo = color;
    }
    else{
        gAlbedo = color_tex.rgba;
    }


}
