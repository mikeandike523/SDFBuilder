// TODO: Add OpenCL kernel code here.
__kernel
void simpleAdd(__global float * A, __global float * B, __global float * C) {
	int id = get_global_id(0);
	C[id] = A[id] + B[id];
}