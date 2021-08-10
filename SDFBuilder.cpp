#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include "debugprint.h"
#include <cstdio>
#include <Windows.h>
#include "Shader.h"
#include <CL/opencl.h>
#include <vector>
#include <string>
#include "Utils.h"
//#include "imgui.h"
//#include "imgui-SFML.h"
#include "openfilename.hpp"
#include "geometry.h"
#include <vector>
#include <thread>
#include <algorithm>
#include "DragDrop.h"
#include <cstring>


//using directives
using std::literals::string_literals::operator""s;
using Geometry::triangle;
std::vector<float> texData;
using glm::vec3;
using glm::mat3;


//application parameters and tunable constants

//application state variables, with tunable initial state
static int winW = 640;
static int winH = 480;
static sf::RenderWindow * window;
static std::wstring inputFilename = L".\\Knight.stl";
static int min_resolution = 64;
static float triangle_thickness = 0.05f;
static vector<triangle> triangles;
static std::vector<std::vector<triangle>> triangle_chunks;
static int lastChunkSize = 256;
static float stl_minX=-1.0;
static float stl_minY=-1.0;
static float stl_minZ=-1.0;
static float stl_maxX=1.0;
static float stl_maxY = 1.0;
static float stl_maxZ = 1.0;
static float stl_rangex = 2.0;
static float stl_rangey = 2.0;
static float stl_rangez = 2.0;
static int worker_running = 0;
static std::thread worker;
static int resx, resy, resz;
constexpr float micro_step = 1.5 * 0.001;
static int sdf_needs_update = 0;
static int stl_loaded = 0;
static float axisX;
static float axisY;
static float axisZ;
static float progress = 0.0f;
static glm::mat3 cam_rotate(1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.1);
static float rotXZ = 0.0f;
constexpr float pi = 3.141592;
constexpr float orbitSpeed = pi / 100.0f;
static float rotZY=0.0;
static float zoom = 7.5;
static mat3 matRotZY;
static std::vector<glm::ivec3> indices;
const int group_amt = Geometry::DATA_SIZE;
int GROUPS;
static int running = 1;
constexpr float zoomSpeed = 1.0;
constexpr float minCameraZoom = 2.0;
static HWND handle;
constexpr float axes_scale = 1.2;
constexpr float ozylim = pi / 2.0 * 0.8;
constexpr float SCALE_DOWN = 4; //octree
constexpr int DO_POOL = 1;
constexpr float FDEF = 0.0f;

//opengl memory locations
static GLuint _id_sdfTex;
static GLuint _id_uniform_axisX;
static GLuint _id_uniform_axisY;
static GLuint _id_uniform_axisZ;
static GLuint _id_uniform_sdf;
static GLuint _id_uniform_loaded;
static GLuint _id_uniform_progress;
static GLuint _id_uniform_winW;
static GLuint _id_uniform_winH;
static GLuint _id_uniform_matRotXZ;
static GLuint _id_uniform_matRotZY;
static GLuint _id_uniform_zoom;
static GLuint _id_uniform_axs;


//ray marching shader
Shader shader;

//old code
GLuint VertexArrayID;
GLuint vertexbuffer;

void build_ui();
void process_stl();
void set_uniforms();
void draw_vertices();
void DragDropCallback(wchar_t* fileName);

void mousedrag(int dX, int dY) {

    float dOXZ = (float)dX * orbitSpeed;
    rotXZ -= dOXZ;
    float dOZY = (float)dY * orbitSpeed;
    rotZY -= dOZY;
    if (fabs(rotZY) > ozylim) {
        rotZY += dOZY;
    }
}

void OpenCL_Test() {
    cl_uint platformCount;
    clGetPlatformIDs(1, NULL, &platformCount);
    cl_platform_id * platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, NULL);


    cl_uint geforce_platform = -1;
    std::string geforce_devicename = "";
    cl_device_id geforce_devid = NULL;

    for (cl_uint i = 0; i < platformCount; i++) {
        cl_device_id device_id;
        cl_uint ret_num_devices;
        cl_int ret = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
        char device_name[256];
        ret = clGetDeviceInfo(device_id, CL_DEVICE_NAME, 512, device_name, NULL);
        std::string devname = std::string(device_name,256);
        std::string devnamelower = Utils::stringToLower(devname);

        if (devnamelower.find("geforce") != -1)
        {
            geforce_platform = i;
            geforce_devicename = devname;
            geforce_devid = device_id;
        }


        CONSOLE_PRINTF_512("device name = %s\n", device_name);
    }

    CONSOLE_PRINTF_512("Nvidia device platform id: %d\nNvidia device name: %s\n",geforce_platform,geforce_devicename.c_str());

    cl_int ret;
    cl_context context = clCreateContext(NULL, 1, &geforce_devid, NULL, NULL, &ret);
    cl_command_queue command_queue = clCreateCommandQueueWithProperties(context,geforce_devid,0,&ret);

     
    float A[256];
    for (int i = 0; i < 256; i++) {
        A[i] = Utils::randf();
    }
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, 256 * sizeof(float), NULL, &ret);
    clEnqueueWriteBuffer(command_queue, bufferA, CL_TRUE, 0, 256 * sizeof(float), (void*)A, 0, NULL, NULL);

    float B[256];
    for (int i = 0; i < 256; i++) {
        B[i] = Utils::randf();
    }
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY, 256 * sizeof(float), NULL, &ret);
    clEnqueueWriteBuffer(command_queue, bufferB, CL_TRUE, 0, 256 * sizeof(float), (void*)B, 0, NULL, NULL);

    float C[256];
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 256 * sizeof(float), NULL, &ret);


    std::string source;
    Utils::file_get_contents_512("test_kernel.cl",source);

    const char * source_cstr = source.c_str();

    cl_program program = clCreateProgramWithSource(context, 1, (const char ** )&source_cstr, NULL, &ret);


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

        // Print the log
        CONSOLE_PRINTF_512("%s\n", log);
    }



    cl_kernel kernel = clCreateKernel(program, "triangle_sdf", &ret);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&bufferA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&bufferB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&bufferC);

    cl_int data_size = 256;
    size_t localws[2] = {16,16};
    size_t globalws[2] = {data_size,data_size};

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, globalws, localws, 0, NULL, NULL);

    ret = clEnqueueReadBuffer(command_queue,bufferC,CL_TRUE,0,data_size*sizeof(float),(void *)C,0,NULL,NULL);

    for (int i = 0; i < data_size; i++) {
        CONSOLE_PRINTF_512("A = %f, B =  %f, C [%d] = %f\n",A[i],B[i],i,C[i]);
    }
   
    //it all works!, got to make sure kernel name matches the function
}

void open_window() {
 /*   static sf::RenderWindow window_local(sf::VideoMode(winW, winH), (const char *)"SDFBuilder", sf::Style::Default, sf::ContextSettings(32));
    window_local.setVerticalSyncEnabled(true);
    window_local.setActive(true);
    window = &window_local;*/


    CONSOLE_PRINTF_512("p_handle: 0x%p\n",&handle);

    static sf::RenderWindow window_local(handle,sf::ContextSettings(32));
    window_local.setVerticalSyncEnabled(true);
    window_local.setActive(true);
    window = &window_local;
 //   DragDrop::activate();

}

void setup_glew() {
    glewInit();
}

void setup_imgui() {

    //ImGui::SFML::Init(*window);


}

void cleanup_imgui() {
    //ImGui::SFML::Shutdown();
}

static unsigned int sample_count = 0;
void calc_field(int group) {
    
    unsigned int num_items = resx * resy * resz; //rgb data where r=g=b=sdf value
    unsigned int num_data_items = num_items * 3;
    const int groups = GROUPS;
    const int group_full = group_amt;
    int group_amt_s = (group == (groups - 1)) ? (num_items-(group_full*(groups-1))): group_full;
 
    const int group_start = group * group_full;
    const int group_end = group_start + group_amt_s;
 //   CONSOLE_PRINTF_512("%d | %d | %d | %d\n",group,group_start,group_end,num_items);

    std::vector<vec3> positions;
    positions.reserve(Geometry::DATA_SIZE);

    for (unsigned int ii = group_start; ii < group_end;ii++) {

                 glm::ivec3 ip = indices[ii];
                 int ix = ip.x;
                 int iy = ip.y;
                 int iz = ip.z;
                float x = -axisX + ((float)ix / (float)resx) * 2.0 * axisX;
                float y = -axisY + ((float)iy / (float)resy) * 2.0 * axisY;
                float z = -axisZ + ((float)iz / (float)resz) * 2.0 * axisZ;



                positions.push_back(vec3(x,y,z));

      }

    while (positions.size() < Geometry::DATA_SIZE) {
        positions.push_back(vec3(0.0));
    }

    float* field;


    if (Geometry::get_pool_amt()>=Geometry::pool_max){
        field = Geometry::get_fields(triangle_chunks, lastChunkSize, positions, triangle_thickness);
        //   CONSOLE_PRINTF_512("%d | %d\n", index*3, texData.size());
        for (unsigned int ii = group_start; ii < group_end; ii++) {
            //  CONSOLE_PRINTF_512("field[%d] = %f\n", ii - group_start, field[ii - group_start]);
            texData[ii * 3 + 0] = field[ii - group_start];
            texData[ii * 3 + 1] = field[ii - group_start];
            texData[ii * 3 + 2] = field[ii - group_start];
        }

        sample_count += group_amt_s;
        progress = (float)(sample_count * 3) / (float)num_data_items;
    }
    else {
        Geometry::get_fields_cpu(positions, triangle_thickness, DO_POOL,group_start,group_end,texData.data(),&sample_count,&progress,group_amt_s,num_data_items,&sdf_needs_update);
    }





}

void process_stl() {
    progress = 0.0f;
    sample_count = 0;
    worker_running = 1;

    std::string inputFilenameA = Utils::convertWtoA(inputFilename);
    FILE* fl = fopen(inputFilenameA.c_str(), "rb");
    fseek(fl, 80, SEEK_SET);
    unsigned long num_trs;
    fread(&num_trs, 4, 1, fl);

    float minx, maxx;
    float miny, maxy;
    float minz, maxz;


    for (unsigned long i = 0; i < num_trs; i++)
    {
        float data[12];
        fread(data, 4, 12, fl);
        triangle tri = triangle(vec3(data[3], data[4], data[5]),
            vec3(data[6], data[7], data[8]),
            vec3(data[9], data[10], data[11]));

        tri.A = Utils::swapYZ(tri.A);
        tri.B = Utils::swapYZ(tri.B);
        tri.C = Utils::swapYZ(tri.C);


        triangles.push_back(tri);


        if (i == 0) {
            minx = tri.minx(); maxx = tri.maxx();
            miny = tri.miny(); maxy = tri.maxy();
            minz = tri.minz(); maxz = tri.maxz();
        }
        else {

            if (tri.minx() < minx) minx = tri.minx();
            if (tri.miny() < miny) miny = tri.miny();
            if (tri.minz() < minz) minz = tri.minz();

            if (tri.maxx() > maxx) maxx = tri.maxx();
            if (tri.maxy() > maxy) maxy = tri.maxy();
            if (tri.maxz() > maxz) maxz = tri.maxz();

        }




        fseek(fl, 2, SEEK_CUR);
    }
    fclose(fl);

    stl_rangex = maxx - minx;
    stl_rangey = maxy - miny;
    stl_rangez = maxz - minz;

    stl_minX = minx;
    stl_maxX = maxx;
    stl_minY = miny;
    stl_maxY = maxy;
    stl_minZ = minz;
    stl_maxZ = maxz;

    const float ranges[] = { stl_rangex,stl_rangey,stl_rangez };
    const float minrange = std::min({ stl_rangex,stl_rangey,stl_rangez });
    if (minrange == ranges[0]) {
        resx = (int)((float)min_resolution);
        resy = (int)((float)min_resolution * stl_rangey / minrange);
        resz = (int)((float)min_resolution * stl_rangez / minrange);
    }
    if (minrange == ranges[1]) {
        resy = (int)((float)min_resolution);
        resx = (int)((float)min_resolution * stl_rangex / minrange);
        resz = (int)((float)min_resolution * stl_rangez / minrange);
    }
    if (minrange == ranges[2]) {
        resz = (int)((float)min_resolution);
        resy = (int)((float)min_resolution * stl_rangey / minrange);
        resx = (int)((float)min_resolution * stl_rangex / minrange);
    }


    CONSOLE_PRINTF_512("stl min x: %f\n", minx);
    CONSOLE_PRINTF_512("stl min y: %f\n", miny);
    CONSOLE_PRINTF_512("stl min z: %f\n", minz);
    CONSOLE_PRINTF_512("stl max x: %f\n", maxx);
    CONSOLE_PRINTF_512("stl max y: %f\n", maxy);
    CONSOLE_PRINTF_512("stl max z: %f\n", maxz);

    axisX = axes_scale * (maxx - minx) / minrange;
    axisY = axes_scale * (maxy - miny) / minrange;
    axisZ = axes_scale * (maxz - minz) / minrange;

    Geometry::set_axes(axisX, axisY, axisZ);

   

    unsigned int num_items = resx * resy * resz; //rgb data where r=g=b=sdf value
    unsigned int num_data_items = num_items * 3;
    GROUPS = num_items / group_amt + 1;
    texData.clear();
    indices.clear();
    //texData.reserve(num_data_items);
   


    for (int ix = 0; ix < resx; ix++) {
        for (int iy = 0; iy < resy; iy++) {
            for (int iz = 0; iz < resz; iz++) {
                indices.push_back(glm::ivec3(ix,iy,iz));
            }
        }
    }

    for (unsigned int ii = 0; ii < indices.size(); ii++) {

        glm::ivec3 ip = indices[ii];
        int ix = ip.x;
        int iy = ip.y;
        int iz = ip.z;
        float x = -axisX + ((float)ix / (float)resx) * 2.0 * axisX;
        float y = -axisY + ((float)iy / (float)resy) * 2.0 * axisY;
        float z = -axisZ + ((float)iz / (float)resz) * 2.0 * axisZ;
      //  float fdef = std::max(Geometry::sdBox(vec3(x, y, z), vec3(axisX, axisY, axisZ)),-Geometry::sdBox(vec3(x, y, z), vec3(axisX / 2.0, axisY / 2.0, axisZ / 2.0)));
        float fdef = FDEF;
        
        texData.push_back(fdef);
        texData.push_back(fdef);
        texData.push_back(fdef);

    }


    for (unsigned long it = 0; it < triangles.size(); it++) {
        triangle tri = triangles[it];
        tri.A = Geometry::normalize_point(tri.A, minx, maxx, miny, maxy, minz, maxz, minrange);
        tri.B = Geometry::normalize_point(tri.B, minx, maxx, miny, maxy, minz, maxz, minrange);
        tri.C = Geometry::normalize_point(tri.C, minx, maxx, miny, maxy, minz, maxz, minrange);
        triangles[it] = tri;
    }

    CONSOLE_PRINTF_512("Building SDF...\n");
    CONSOLE_PRINTF_512("axisX: %f\n",axisX);
    CONSOLE_PRINTF_512("axisY: %f\n", axisY);
    CONSOLE_PRINTF_512("axisZ: %f\n", axisZ);
    CONSOLE_PRINTF_512("resX: %d\n", resx);
    CONSOLE_PRINTF_512("resY: %d\n", resy);
    CONSOLE_PRINTF_512("resZ: %d\n", resz);

    triangle_chunks = Geometry::chunk_triangles(triangles, &lastChunkSize);

    CONSOLE_PRINTF_512("\nDone.\n");

    Geometry::octree::init(axisX, axisY, axisZ, resx, resy, resz, SCALE_DOWN);
    Geometry::octree::populate(triangles);


    stl_loaded = 1;
    sdf_needs_update = 1;

    for (int i = 0; i < GROUPS; i++) {
        if (!running)
            break;
        calc_field(i);
        sdf_needs_update = 1;
    }
    
    CONSOLE_PRINTF_512("waiting on threads.\n");
    while (Geometry::get_pool_amt() > 0) { }
    CONSOLE_PRINTF_512("done.\n");

    worker_running = 0;
}

                           
int event_loop() {

    DragDrop::processEvents();

    static int dragging = 0; //dragging the mouse
    static sf::Vector2i old_mouse_position = sf::Vector2i(-1, -1);

    sf::Event event;
    while (window->pollEvent(event))
    {

    //    ImGui::SFML::ProcessEvent(event);

        if (event.type == sf::Event::Closed)
        {
            // end the program
            return 0;
        }


        else if (event.type == sf::Event::Resized)
        {
    
            winW = event.size.width;
            winH = event.size.height;

            glViewport(0, 0, event.size.width, event.size.height);
        }

        else if (event.type == sf::Event::MouseButtonPressed) {

            dragging = 1;
        }
        else if (event.type == sf::Event::MouseButtonReleased) {

            dragging = 0;
            old_mouse_position = sf::Vector2i(-1,-1);

        }

        else if (event.type == sf::Event::MouseWheelMoved)
        {
            float delta = zoomSpeed * (float)event.mouseWheel.delta;
            zoom -= delta;
            if (zoom <= minCameraZoom) {
                zoom += delta;
            }
        }

        

    }

    if (dragging) {
        sf::Vector2i mouse_position = sf::Mouse::getPosition();
        if (old_mouse_position.x > 0) {

            mousedrag(mouse_position.x - old_mouse_position.x, mouse_position.y - old_mouse_position.y);

        }
        old_mouse_position = mouse_position;

    }

    return 1;

}

void draw_loop() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //build_ui();
    glUseProgram(shader.ID);
    set_uniforms();
    draw_vertices();
    //draw the arrays
    glUseProgram(0);
   // ImGui::SFML::Render(*window);
    window->display();
}


void build_ui() {
    /*
    //ImGui::SetNextWindowSize(ImVec2(500.0f, 200.0f));
   
    ImGui::Begin("SDFBuilder",0, ImGuiWindowFlags_AlwaysAutoResize); // begin window

    ImGui::InputInt("Minimum Resolution", &min_resolution);
    ImGui::InputFloat("Triangle Thickness", &triangle_thickness);

    ImGui::Text(Utils::convertWtoA(inputFilename).c_str()); ImGui::SameLine();
    if (ImGui::Button("Select File")) {
        inputFilename = openfilename::openfilename();
        if (inputFilename == L"") {
            inputFilename = L".\\Knight.stl";
        }
    }

    if (!worker_running) {
        if (ImGui::Button("Build SDF")) {
         
            worker = std::thread(process_stl);
            worker.detach();
            
        }
    }


    ImGui::End(); // end window
    */

}

void init_texture() {
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_id_sdfTex);
    glBindTexture(GL_TEXTURE_3D, _id_sdfTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
}

void update_sdf() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, _id_sdfTex);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, resz, resy, resx, 0, GL_RGB,
        GL_FLOAT, (float *)texData.data());
    sdf_needs_update = 0;
    //CONSOLE_PRINTF_512("Updated SDF texture\n");
}


void retrieve_uniform_ids() {
    _id_uniform_axisX = glGetUniformLocation(shader.ID, "axisX");
    _id_uniform_axisY = glGetUniformLocation(shader.ID, "axisY");
    _id_uniform_axisZ = glGetUniformLocation(shader.ID, "axisZ");
    _id_uniform_sdf = glGetUniformLocation(shader.ID, "sdf");
    _id_uniform_loaded = glGetUniformLocation(shader.ID, "loaded");
    _id_uniform_progress = glGetUniformLocation(shader.ID, "progress");
    _id_uniform_winW = glGetUniformLocation(shader.ID, "winW");
    _id_uniform_winH = glGetUniformLocation(shader.ID, "winH");
    _id_uniform_matRotXZ = glGetUniformLocation(shader.ID, "matRotXZ");
    _id_uniform_matRotZY = glGetUniformLocation(shader.ID, "matRotZY");
    _id_uniform_zoom = glGetUniformLocation(shader.ID, "zoom");
    _id_uniform_axs= glGetUniformLocation(shader.ID, "axs");
}


void set_uniforms() {
    glUniform1f(_id_uniform_axisX, axisX);
    glUniform1f(_id_uniform_axisY, axisY);
    glUniform1f(_id_uniform_axisZ, axisZ);
    glUniform1i(_id_uniform_sdf, 0);
    glUniform1i(_id_uniform_loaded, stl_loaded);
    glUniform1f(_id_uniform_progress, progress);
    glUniform1f(_id_uniform_winW, (float)winW);
    glUniform1f(_id_uniform_winH, (float)winH);
    cam_rotate = Geometry::rotXZMat3(rotXZ);
    glUniformMatrix3fv(_id_uniform_matRotXZ,1,GL_FALSE,&cam_rotate[0][0]);
    glUniform1f(_id_uniform_zoom, zoom);
    matRotZY = Geometry::rotZYMat3(rotZY);
    glUniformMatrix3fv(_id_uniform_matRotZY, 1, GL_FALSE, &matRotZY[0][0]);
    glUniform1f(_id_uniform_axs, axes_scale);
}

void setup_vertices() {
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
     -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f,  -1.0f, 0.0f,
    };
    // This will identify our vertex buffer

    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
}

void draw_vertices() {
      glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0);
}

//primary logic of the program, will not be linked to entry point
int main() {

  

    srand((unsigned int)Utils::now_millis());

    open_window();
    setup_glew();
    setup_vertices();
   // setup_imgui();
    init_texture();


    if (shader.load("v1.vert", "f1.frag") != 0) {
        std::exit(1);
    }

    Geometry::init_opencl();

   
    
    


    retrieve_uniform_ids();

    sf::Clock deltaClock;

   // OpenCL_Test();

    
   // DragDropCallback((wchar_t *)L".\\Knight.stl");

    while (running) {
    
        running = event_loop();
      //  ImGui::SFML::Update(*window, deltaClock.restart());
        if (sdf_needs_update)
            update_sdf();
        draw_loop();
        

    }

    


   // cleanup_imgui();
    Geometry::quit_opencl();
    while (worker_running) {}

    return 0;
}


void DragDropCallback(wchar_t * fileName) {
    if (!worker_running) {
        inputFilename = std::wstring(fileName);
        worker = std::thread(process_stl);
        worker.detach();
    }
}

//open a window without console
int APIENTRY wWinMain(_In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     LPWSTR    lpCmdLine,
    _In_     int       nCmdShow)
{
   
    DragDrop::init(hInstance, hPrevInstance,NULL,1,&handle,DragDropCallback);
   
	return main();
    return 0;
}

