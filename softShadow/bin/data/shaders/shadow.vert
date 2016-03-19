#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform vec4 globalColor;

in vec4  position;
in vec4  color;
in vec3  normal;
in vec2  texcoord;

out vec4 colorVarying;
out vec2 texCoordVarying;
out vec3 normalVarying;
out vec4 positionVarying;

uniform mat4 viewMatrix;
uniform mat4 lightSpaceMatrix;
uniform vec3 lightPosition;
uniform int isPerspective;

out vec3 v_normalVarying;
out vec4 v_positionVarying;
out vec4 v_shadowCoord;
out vec3 v_lightPosition;

void main() {
    normalVarying = normal;
    positionVarying = position;
    
    mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));
    v_normalVarying = normalize(vec3(normalMatrix * normal));
    v_positionVarying = modelViewMatrix * position;
    
    v_shadowCoord = lightSpaceMatrix * v_positionVarying;
    
    if(isPerspective == 1){
        v_lightPosition = vec3(viewMatrix * vec4(lightPosition, 1.0));
    }else{
        v_lightPosition = vec3(viewMatrix * vec4(lightPosition, 0.0));
    }
    
    gl_Position = modelViewProjectionMatrix * position;
    
}