// fragment shader

#version 150

// this is how we receive the texture
uniform sampler2DRect tex0;
uniform float videoW;
uniform float videoH;
uniform float windowW;
uniform float windowH;
uniform vec2 mousePos;

out vec4 outputColor;

void main()
{
    // gl_FragCoord contains the window relative coordinate for the fragment.
    float r = gl_FragCoord.x / windowW;
    float g = gl_FragCoord.y / windowH;
    float b = 1.0;
    float a = 1.0;
    outputColor = vec4(r, g, b, a);	
	
	// Need to remap 
	// gl_FragCoord is in OF coordinates
	vec2 pick = vec2(gl_FragCoord.x / windowW * videoW, videoH - gl_FragCoord.y / windowH * videoH);
    outputColor = texture(tex0, pick);
}