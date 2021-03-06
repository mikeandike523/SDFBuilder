float dot2(float3 v) { return v.x* v.x + v.y * v.y +v.z * v.z; }
    const float signf(float f) { return f >= 0.0 ? 1.0 : -1.0; }
    const float minf(float a, float b) { return a < b ? a : b; };
    const float maxf(float a, float b) { return a < b ? b : a; };
    float clampf(float f, float low, float high) {
        return minf(maxf(f, low), high);
    }

    float sdTriangle(float3 v1, float3 v2, float3 v3, float3 p, float tri_thickness)
    {
        // prepare data    
        float3 v21 = v2 - v1; float3 p1 = p - v1;
        float3 v32 = v3 - v2; float3 p2 = p - v2;
        float3 v13 = v1 - v3; float3 p3 = p - v3;
        float3 nor = cross(v21, v13);

        return sqrt( // inside/outside test    
            (signf(dot(cross(v21, nor), p1)) +
                signf(dot(cross(v32, nor), p2)) +
                signf(dot(cross(v13, nor), p3)) < 2.0)
            ?
            // 3 edges    
            minf(minf(
                dot2(v21 * clampf(dot(v21, p1) / dot2(v21), 0.0, 1.0) - p1),
                dot2(v32 * clampf(dot(v32, p2) / dot2(v32), 0.0, 1.0) - p2)),
                dot2(v13 * clampf(dot(v13, p3) / dot2(v13), 0.0, 1.0) - p3))
            :
            // 1 face    
            dot(nor, p1) * dot(nor, p1) / dot2(nor)) - tri_thickness;
    }



    __kernel
    void triangle_sdf(__global float * Ax, __global float * Ay, __global float * Az, 
        __global float* Bx, __global float* By, __global float* Bz,
        __global float* Cx, __global float* Cy, __global float* Cz,
        __global float * x, __global float * y, __global float * z
        , __global float * thickness, __global float * field) {

        int id = get_global_id(0);
        field[id]=sdTriangle((float3)(Ax[id],Ay[id],Az[id]), (float3)(Bx[id], By[id], Bz[id]), (float3)(Cx[id], Cy[id], Cz[id]), (float3)(x[0],y[0],z[0]), thickness[0]);
        //field[id] = sqrt(x[0] * x[0] + y[0] * y[0] + z[0] * z[0]) - 0.4;
      
    }



