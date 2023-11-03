#version 400
//in float logz;
out vec4 fragColor;

void main() 
{		

	//gl_FragDepth = logz;
	//gl_FragDepth = log(logz);
	fragColor = vec4(0.0, 0.0, 1.0, 1.0);
}
