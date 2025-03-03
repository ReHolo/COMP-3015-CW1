#version 460
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

// BASC核心结构体
uniform struct LightInfo {
    vec3 L;   
    vec3 La;  
} Light;

uniform struct MaterialInfo {
    vec3 Ka; 
    vec3 Kd; 
    vec3 Ks; 
    float Shininess;
} Material;


uniform sampler2D planeBaseColorMap;    // 5: wood.jpg
uniform sampler2D planeNormalMap;       // 6: wood_normal.jpg
uniform sampler2D planeRoughnessMap;    // 7: wood_roughness.jpg
uniform sampler2D planeOcclusionMap;    // 8: wood_occlusion.jpg


uniform bool DebugMode = false; 
uniform int DisplayChannel = 0; 

vec3 blinnPhong(vec3 pos, vec3 n) {
    vec3 s = normalize(-pos); 
    vec3 ambient = Light.La * Material.Ka;
    
    float sDotN = max(dot(s, n), 0.0);
    vec3 diffuse = Material.Kd * sDotN * Light.L;
    
    vec3 spec = vec3(0.0);
    if (sDotN > 0.0) {
        vec3 v = normalize(-pos);
        vec3 h = normalize(s + v);
        spec = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shininess) * Light.L;
    }
    
    return ambient + diffuse + spec;
}

vec3 debugDisplay(vec2 tc) {
    
    switch (DisplayChannel) {
        case 1: return texture(planeNormalMap, tc).rgb;
        case 2: return texture(planeRoughnessMap, tc).rrr;
        case 3: return texture(planeOcclusionMap, tc).rrr;
        default: return blinnPhong(Position, normalize(Normal));
    }
}

void main() {
    
    vec3 N = normalize(Normal);

    
      vec3 normalTex = texture(planeNormalMap, TexCoord).rgb * 2.0 - 1.0;
    

    
    vec3  baseColor   = texture(planeBaseColorMap, TexCoord).rgb;
    //float metallic    = texture(planeMetallicMap,  TexCoord).r;
    float roughness   = texture(planeRoughnessMap, TexCoord).r;
    float ao          = texture(planeOcclusionMap, TexCoord).r;
    

    
    MaterialInfo mat = Material;
    
    mat.Kd = baseColor; 
    

    
    vec3 color = blinnPhong(Position, N);

    
    color = mix(color, color * ao, 0.75); 

    vec3 finalColor = debugDisplay(TexCoord);
    FragColor = vec4(finalColor, 1.0);

    //FragColor = vec4(color,1.0);
    FragColor = vec4(baseColor, 1.0);
}

