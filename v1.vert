attribute vec3 gpu_Vertex;

void main(void)
{


	gl_Position=vec4(gpu_Vertex, 1.0);
}