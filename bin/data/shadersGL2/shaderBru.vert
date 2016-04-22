#version 120

// this is coming from our C++ code
uniform vec2 mousePos;
uniform float mouseRange;

varying vec2 texCoordVarying;

void main()
{
    // copy position so we can work with it.
    //vec2 pos = gl_MultiTexCoord0.xy;
    vec4 pos = gl_Vertex;
    pos.xy = gl_MultiTexCoord0.xy;
    
    // direction vector from mouse position to vertex position.
    vec2 dir = pos.xy - mousePos;
    
    // distance between the mouse position and vertex position.
    float dist =  sqrt(dir.x * dir.x + dir.y * dir.y);
    
    // check vertex is within mouse range.
    if(dist > 0.0 && dist < mouseRange)
    {		
		    // normalise distance between 0 and 1.
    		float distNorm = dist / mouseRange;
        
    		// flip it so the closer we are the greater the repulsion.
    		distNorm = 1.0 - distNorm;
		
        // make the direction vector magnitude fade out the further it gets from mouse position.
        dir *= distNorm;
        
    		// add the direction vector to the vertex position.
    		pos.x += dir.x;
    		pos.y += dir.y;
	}
	texCoordVarying = pos.xy;
    
	// finally set the pos to be that actual position rendered
	gl_Position = gl_ModelViewProjectionMatrix * pos;
}
