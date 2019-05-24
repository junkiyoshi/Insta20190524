#version 150

uniform float time;
uniform vec2 resolution;
uniform sampler2DRect tex1;
uniform sampler2DRect tex2;

in vec2 texCoordVarying;
out vec4 outputColor;

void main() {

  vec4 t1 = texture(tex1, vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y));
  vec4 t2 = texture(tex2, vec2(gl_FragCoord.x, resolution.y - gl_FragCoord.y));
 
  outputColor = t1 * t2;
}