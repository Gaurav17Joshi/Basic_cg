#version 330 core

in vec3 mynormal; 
in vec4 myvertex; 

uniform mat4 modelview;

out vec4 fragColor;

uniform vec3 color;

const int numLights = 10; 
uniform bool enablelighting;
uniform vec4 lightposn[numLights]; 
uniform vec4 lightcolor[numLights];
uniform int numused;

uniform vec4 ambient; 
uniform vec4 diffuse; 
uniform vec4 specular; 
uniform vec4 emission; 
uniform float shininess; 

void main (void) 
{       
    if (enablelighting) {       
        // YOUR CODE FOR HW 2 HERE - SOLUTION:
        
        // Transform normal and vertex to eye space
        vec3 normal = normalize(mat3(modelview) * mynormal);
        vec4 vertex_eye = modelview * myvertex;
        vec3 eyepos = vec3(vertex_eye);
        
        // View direction (towards eye in eye space, eye is at origin)
        vec3 viewdir = normalize(-eyepos);
        
        // Start with ambient and emission
        vec4 finalcolor = ambient + emission;
        
        // Loop through all lights
        for (int i = 0; i < numused; i++) {
            vec3 lightdir;
            
            // Check if directional or point light
            if (lightposn[i].w == 0.0) {
                // Directional light (w = 0)
                lightdir = normalize(vec3(lightposn[i]));
            } else {
                // Point light (w = 1)
                lightdir = normalize(vec3(lightposn[i]) - eyepos);
            }
            
            // Compute diffuse component
            float nDotL = max(dot(normal, lightdir), 0.0);
            vec4 diffuseComponent = diffuse * lightcolor[i] * nDotL;
            
            // Compute specular component
            vec3 halfvec = normalize(lightdir + viewdir);
            float nDotH = max(dot(normal, halfvec), 0.0);
            vec4 specularComponent = specular * lightcolor[i] * pow(nDotH, shininess);
            
            // Add contributions from this light
            finalcolor += diffuseComponent + specularComponent;
        }
        
        fragColor = finalcolor; 
    } else {
        fragColor = vec4(color, 1.0f); 
    }
}
