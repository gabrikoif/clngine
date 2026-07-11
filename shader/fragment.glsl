#version 330 core
out vec4 FragColor;

in vec3 FragPos;     // Passed from vertex shader
in vec3 Normal;      // Passed from vertex shader
in vec3 objectColor; // Per-vertex color passed from vertex shader

uniform vec3 lightPos;   
uniform vec3 viewPos;    
uniform vec3 lightColor;

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // ==========================================
    // 1. MAIN LIGHT SOURCE (Blinn-Phong)
    // ==========================================
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // Ambient
    float ambientStrength = 0.15; // Kept low because the fill light adds ambient brightness
    vec3 ambient = ambientStrength * lightColor;
  	
    // Diffuse 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular (Blinn-Phong uses the Halfway Vector)
    float specularStrength = 0.7;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    // Note: Blinn-Phong requires roughly double the shininess exponent (32 -> 64) for a similar look
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0); 
    vec3 specular = specularStrength * spec * lightColor;  
        
    // ==========================================
    // 2. SECONDARY FILL LIGHT (Fixes the harsh dark side)
    // ==========================================
    // A faint, overhead directional light mimicking sky/room bounce
    vec3 fillLightDir = normalize(vec3(-0.5, 1.0, -0.5)); 
    vec3 fillLightColor = vec3(0.2, 0.2, 0.25); // Soft, slightly cool tone
    float fillDiff = max(dot(norm, fillLightDir), 0.0) * 0.4; // 40% intensity
    vec3 fillDiffuse = fillDiff * fillLightColor;

    // ==========================================
    // 3. COMBINE & OUTPUT
    // ==========================================
    vec3 totalLighting = ambient + diffuse + specular + fillDiffuse;
    vec3 result = totalLighting * objectColor;
    
    FragColor = vec4(result, 1.0);
}