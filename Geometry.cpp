#include "Geometry.h"
#include "Utils.h"
#include <CL/opencl.h>
#include "debugprint.h"
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <string>
#include <Windows.h>
#include <cmath>
using glm::vec3;
using Geometry::triangle;
using glm::mat3;
using glm::length;
using std::max;
using std::min;
namespace Geometry {
    
    static float axisX, axisY, axisZ;
    void set_axes(float _axisX,float _axisY,float _axisZ) {
        axisX = _axisX;
        axisY = _axisY;
        axisZ = _axisZ;
    }


    vec3 maxv(vec3 a, vec3 b) {
        return vec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
    }

    float sdBox(vec3 p, vec3 b)
    {
        vec3 q = abs(p) - b;
        return length(maxv(q, vec3(0.0))) + min(max(q.x, max(q.y, q.z)), 0.0);
    }


    constexpr float micro_step = 1.5 * 0.001;
  
    float dot2(glm::vec3 v) { return glm::dot(v, v); }
    constexpr float signf(float f) { return f >= 0.0 ? 1.0 : -1.0; }
    constexpr float minf(float a, float b) { return a < b ? a : b; };
    constexpr float maxf(float a, float b) { return a < b ? b : a; };
    float clampf(float f, float low, float high) {
        return minf(maxf(f, low), high);
    }

    static float Ax[DATA_SIZE], Ay[DATA_SIZE], Az[DATA_SIZE];
    static float Bx[DATA_SIZE], By[DATA_SIZE], Bz[DATA_SIZE];
    static float Cx[DATA_SIZE], Cy[DATA_SIZE], Cz[DATA_SIZE];
    static float x[DATA_SIZE], y[DATA_SIZE], z[DATA_SIZE];
    static float thickness[1];
    static float field[DATA_SIZE];
    static cl_mem bAx, bAy, bAz, bBx, bBy, bBz, bCx, bCy, bCz, bx, by, bz, bthickness, bfield, bds, baxes;
    static int ds[1];
    static float axes[3];


    static cl_command_queue cq;
    static cl_kernel kernel;

    float sdTriangle(vec3 v1, vec3 v2, vec3 v3, vec3 p, float tri_thickness)
    {
        // prepare data    
        vec3 v21 = v2 - v1; vec3 p1 = p - v1;
        vec3 v32 = v3 - v2; vec3 p2 = p - v2;
        vec3 v13 = v1 - v3; vec3 p3 = p - v3;
        vec3 nor = cross(v21, v13);

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



	float triangle_sdf(Geometry::triangle tri, glm::vec3 pos, float thickness) {
	
        return sdTriangle(tri.A, tri.B, tri.C, pos, thickness);
	}

    

    vec3 normalize_point(vec3 p,float minx, float maxx, float miny, float maxy, float minz, float maxz, float minrange) {

        float axisX = (maxx - minx) / minrange;
        float axisY = (maxy - miny) / minrange;
        float axisZ = (maxz - minz) / minrange;

        return vec3(-axisX+(2.0f*axisX)*(p.x-minx)/(maxx-minx),
            -axisY + (2.0f * axisY) * (p.y - miny) / (maxy - miny),
            -axisZ + (2.0f * axisZ) * (p.z - minz) / (maxz - minz));

    }


    void init_opencl() {
    
        cl_uint platformCount;
        clGetPlatformIDs(1, NULL, &platformCount);
        cl_platform_id* platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * platformCount);
        clGetPlatformIDs(platformCount, platforms, NULL);


        cl_uint geforce_platform = -1;
        std::string geforce_devicename = "";
        cl_device_id geforce_devid = NULL;

        for (cl_uint i = 0; i < platformCount; i++) {
            cl_device_id device_id;
            cl_uint ret_num_devices;
            cl_int ret = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
            char device_name[DATA_SIZE];
            ret = clGetDeviceInfo(device_id, CL_DEVICE_NAME, 512, device_name, NULL);
            std::string devname = std::string(device_name, DATA_SIZE);
            std::string devnamelower = Utils::stringToLower(devname);

            if (devnamelower.find("geforce") != -1)
            {
                geforce_platform = i;
                geforce_devicename = devname;
                geforce_devid = device_id;
            }

         
                CONSOLE_PRINTF_512("device name = %s\n", device_name);
        }

        CONSOLE_PRINTF_512("Nvidia device platform id: %d\nNvidia device name: %s\n", geforce_platform, geforce_devicename.c_str());

        cl_int ret;
        cl_context context = clCreateContext(NULL, 1, &geforce_devid, NULL, NULL, &ret);
        cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, geforce_devid, 0, &ret);


        //make the buffers here
        
       bAx= clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE * sizeof(float), NULL, &ret);
        bAy = clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE * sizeof(float), NULL, &ret);
       bAz= clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE * sizeof(float), NULL, &ret);
       bBx = clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE * sizeof(float), NULL, &ret);
        bBy = clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE * sizeof(float), NULL, &ret);
       bBz = clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE * sizeof(float), NULL, &ret);
       bCx = clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE * sizeof(float), NULL, &ret);
      bCy = clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE * sizeof(float), NULL, &ret);
       bCz = clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE * sizeof(float), NULL, &ret);
       bx = clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE * sizeof(float), NULL, &ret);
         by = clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE * sizeof(float), NULL, &ret);
       bz = clCreateBuffer(context, CL_MEM_READ_ONLY, DATA_SIZE  * sizeof(float), NULL, &ret);
       bthickness = clCreateBuffer(context, CL_MEM_READ_ONLY, 1 * sizeof(float), NULL, &ret);
         bfield = clCreateBuffer(context, CL_MEM_READ_WRITE,DATA_SIZE * sizeof(float), NULL, &ret);
         bds = clCreateBuffer(context, CL_MEM_READ_ONLY, 1 * sizeof(int), NULL, &ret);
         baxes = clCreateBuffer(context, CL_MEM_READ_ONLY, 3 * sizeof(float), NULL, &ret);


        std::string source;
     
        Utils::file_get_contents_512("triSDFON2.cl", source);

        const char* source_cstr = source.c_str();

        cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_cstr, NULL, &ret);



        ret = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

        //https://stackoverflow.com/a/9467325/5166365
        if (ret == CL_BUILD_PROGRAM_FAILURE) {
            // Determine the size of the log
            size_t log_size;
            clGetProgramBuildInfo(program, geforce_devid, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

            // Allocate memory for the log
            char* log = (char*)malloc(log_size);

            // Get the log
            clGetProgramBuildInfo(program, geforce_devid, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

            std::wstring logw;
            std::string loga = std::string(log);
            Utils::StringToWString(logw, loga);
            OutputDebugStringW((LPCWSTR)logw.c_str());
        }


        kernel = clCreateKernel(program, "triangle_sdf", &ret);
        clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&bAx);
        clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&bAy);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&bAz);
        clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&bBx);
        clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&bBy);
        clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&bBz);
        clSetKernelArg(kernel, 6, sizeof(cl_mem), (void*)&bCx);
        clSetKernelArg(kernel, 7, sizeof(cl_mem), (void*)&bCy);
        clSetKernelArg(kernel, 8, sizeof(cl_mem), (void*)&bCz);
        clSetKernelArg(kernel, 9, sizeof(cl_mem), (void*)&bx);
        clSetKernelArg(kernel, 10, sizeof(cl_mem), (void*)&by);
        clSetKernelArg(kernel, 11, sizeof(cl_mem), (void*)&bz);
        clSetKernelArg(kernel, 12, sizeof(cl_mem), (void*)&bthickness);
        clSetKernelArg(kernel, 13, sizeof(cl_mem), (void*)&bfield);
        clSetKernelArg(kernel, 14, sizeof(cl_mem), (void*)&bds);
        clSetKernelArg(kernel, 15, sizeof(cl_mem), (void*)&baxes);

        cq = command_queue;


    }
    void quit_opencl() {}
    std::vector<std::vector<triangle>> chunk_triangles(std::vector<triangle> & triangles, int * last_chunk_size) {
        std::vector<std::vector<triangle>> chunks;
        for (unsigned int i = 0; i < triangles.size();i++) {
            if ((i % DATA_SIZE) == 0)
            {
                chunks.push_back(std::vector<triangle>());
                
            }
            chunks[chunks.size() - 1].push_back(triangles[i]);
        }
        *last_chunk_size = chunks[chunks.size() - 1].size();
        CONSOLE_PRINTF_512("last chunk size %d\n", chunks[chunks.size() - 1].size());
        while (chunks[chunks.size() - 1].size() < DATA_SIZE) {
            chunks[chunks.size() - 1].push_back(triangle(vec3(0.0),vec3(0.0),vec3(0.0)));
        }
        CONSOLE_PRINTF_512("num tris %d\n", triangles.size());
        CONSOLE_PRINTF_512("num chunks %d\n",chunks.size());
        CONSOLE_PRINTF_512("last chunk size %d\n", chunks[chunks.size() - 1].size());
        return chunks;
    }

   
    mat3 rotXZMat3(float angle) {
        return mat3(cos(angle), 0.0, sin(angle), 0.0, 1.0, 0.0, cos(angle + pi / 2.0), 0.0, sin(angle + pi / 2.0));
    }

    mat3 rotZYMat3(float angle) {
        return mat3(1.0,0.0,0.0,0.0,sin(angle+pi/2.0),cos(angle+pi/2.0),0.0,sin(angle),cos(angle));
    }
    float * get_fields(std::vector<std::vector<triangle>>& chunks, int lastChunkSize, std::vector<vec3>& positions, float tri_thickness) {
    
        for (int i = 0; i < DATA_SIZE; i++) {
            x[i] = positions[i].x;
            y[i] = positions[i].y;
            z[i] = positions[i].z;
        }
        for (int i = 0; i < DATA_SIZE; i++) {
            field[i] = 2048.0;
            //field[i] = micro_step*2.0;
        }
        axes[0] = axisX;
        axes[1] = axisY;
        axes[2] = axisZ;
        thickness[0] = tri_thickness;
        ds[0] = DATA_SIZE;
        // CONSOLE_PRINTF_512("last chunk size %d\n", lastChunkSize);

        for (unsigned int cn = 0; cn < chunks.size(); cn++) {
            std::vector<triangle>& chunk = chunks[cn];

            for (unsigned int it = 0; it < chunk.size(); it++) {
                triangle tri = chunk[it];

                Ax[it] = tri.A.x;
                Ay[it] = tri.A.y;
                Az[it] = tri.A.z;

                Bx[it] = tri.B.x;
                By[it] = tri.B.y;
                Bz[it] = tri.B.z;

                Cx[it] = tri.C.x;
                Cy[it] = tri.C.y;
                Cz[it] = tri.C.z;
            }

            clEnqueueWriteBuffer(cq, bAx, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)Ax, 0, NULL, NULL);
            clEnqueueWriteBuffer(cq, bAy, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)Ay, 0, NULL, NULL);
            clEnqueueWriteBuffer(cq, bAz, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)Az, 0, NULL, NULL);
            clEnqueueWriteBuffer(cq, bBx, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)Bx, 0, NULL, NULL);
            clEnqueueWriteBuffer(cq, bBy, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)By, 0, NULL, NULL);
            clEnqueueWriteBuffer(cq, bBz, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)Bz, 0, NULL, NULL);
            clEnqueueWriteBuffer(cq, bCx, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)Cx, 0, NULL, NULL);
            clEnqueueWriteBuffer(cq, bCy, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)Cy, 0, NULL, NULL);
            clEnqueueWriteBuffer(cq, bCz, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)Cz, 0, NULL, NULL);

            clEnqueueWriteBuffer(cq, bx, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)x, 0, NULL, NULL);
            clEnqueueWriteBuffer(cq, by, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)y, 0, NULL, NULL);
            clEnqueueWriteBuffer(cq, bz, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)z, 0, NULL, NULL);

            clEnqueueWriteBuffer(cq, bthickness, CL_TRUE, 0, 1 * sizeof(float), (void*)thickness, 0, NULL, NULL);
            clEnqueueWriteBuffer(cq, bfield, CL_TRUE, 0, DATA_SIZE * sizeof(float), (void*)field, 0, NULL, NULL);
            clEnqueueWriteBuffer(cq, bds, CL_TRUE, 0, 1 * sizeof(int), (void*)ds, 0, NULL, NULL);

            clEnqueueWriteBuffer(cq, baxes, CL_TRUE, 0,3 * sizeof(float), (void*)axes, 0, NULL, NULL);

            cl_int ret;
            cl_int data_size = DATA_SIZE;
            size_t localws[2] = { data_size / GLOBAL_LOCAL_RATIO ,data_size / GLOBAL_LOCAL_RATIO };
            size_t globalws[2] = { data_size,data_size };

            ret = clEnqueueNDRangeKernel(cq, kernel, 2, NULL, globalws, localws, 0, NULL, NULL);
            ret = clEnqueueReadBuffer(cq, bfield, CL_TRUE, 0, data_size * sizeof(float), (void*)field, 0, NULL, NULL);
            
            
            for (unsigned int i = 0; i < DATA_SIZE; i++) {
                if (field[i]==2048.0) {
                    field[i] == 0.0;
                }
            }
            
            



        }
    



    
        return field;
    }
}