#version 420
uniform int width;
uniform int height;
uniform float time;
uniform layout(binding=3, rgba8ui) writeonly uimage2D fractal_texture;


void main()
{
	//we convert this vertex ID to 2D
	ivec2 i = ivec2(gl_VertexID % width, gl_VertexID / width);
	vec2 uv = vec2(i) * vec2(1.0 / float(width), 1.0 / float(height));
	


	//fractal rendering, in the end we have a color, all we care about
		float n = 0.0;
	    vec2 c = vec2(-0.745, 0.186) +  (uv - vec2(0.5,0.5))*(2.0+ 1.7*cos(1.8*time));
        vec2 z = vec2(0.0);
        int M =128;
        for (int i = 0; i<M; i++)
        {
			z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
			if (dot(z, z) > 2) break;

			n++;
        }
        vec3 bla = vec3(0,0,0.0);
        vec3 blu = vec3(0.2,0.2,0.8);
        vec4 color;
        if( n >= 0 && n <= M/2-1 ) { color = vec4( mix( vec3(0.2, 0.1, 0.4), blu, n / float(M/2-1) ), 1.0) ;  }
        if( n >= M/2 && n <= M ) { color = vec4( mix( blu, bla, float(n - M/2 ) / float(M/2) ), 1.0) ;  }
		
	imageStore(fractal_texture, i , uvec4(color * 255.0f));
}
