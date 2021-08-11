#include "BourkeExport.h"
#include <string>
#include "Utils.h"
#include <cstdio>
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include "debugprint.h"
#include <cstdint>
#include <vector>

namespace BourkeExport {

	//https://stackoverflow.com/a/1001373/5166365
	int is_big_endian(void)
	{
		union {
			uint32_t i;
			char c[4];
		} bint = { 0x01020304 };

		return (bint.c[0] == 1)?1:0;
	}


	std::wstring adjust_name(std::wstring fn) {
		int L = fn.length();
		fn[L - 3] = L's';
		fn[L - 2] = L'd';
		fn[L - 1] = L'f';
		return fn;
	}

	void fwrite_line(FILE * ofl,char * buffer512) {
		int index = 0;
		while (buffer512[index] != '\0'&&index<512) {
			char c = buffer512[index];
			fwrite(&c,1,1,ofl);
			index++;
		}
	}


	


	void export_sdf(std::wstring fn,glm::vec3 axes, glm::ivec3 res, float * texels) 
	{
		std::wstring fn2 = adjust_name(fn);
		std::string fn2a = Utils::convertWtoA(fn2);
		FILE* ofile = fopen(fn2a.c_str(),"wb");
		if (ofile != NULL) {
			char line[512];
			snprintf(line,512,"Signed Distance Field\n");
			fwrite_line(ofile,line);
			snprintf(line,512, "%d %d %d\n", res.x, res.y, res.z);
			fwrite_line(ofile, line);
			float cellx = (2.0f * axes.x) / ((float)res.x);
			float celly= (2.0f * axes.y) / ((float)res.y);
			float cellz = (2.0f * axes.z) / ((float)res.z);
			snprintf(line,512, "%f %f %f\n", cellx, celly, cellz);
			fwrite_line(ofile, line);
			snprintf(line,512, "%f %f %f\n", -axes.x, -axes.y, -axes.z);
			fwrite_line(ofile, line);
			snprintf(line,512, "%d %d\n", 8*sizeof(float), 1-is_big_endian());
			fwrite_line(ofile, line);
		}
		
		unsigned int Nx = res.x;
		unsigned int Ny = res.y;
		unsigned int Nz = res.z;

		for (unsigned int z = 0; z < Nz; z++) {
			for (unsigned int y = 0; y < Ny; y++) {
				for (unsigned int x = 0; x < Nx; x++) {
					unsigned int item_index = x * Ny * Nz + y * Nz + z;
					unsigned int texel_index = item_index * 3;
					float sdf_value = texels[texel_index];
					fwrite(&sdf_value, 1, sizeof(float), ofile);

				}
			}
		}

		if(ofile!=NULL)
		fclose(ofile);
	}

	void import_sdf(std::wstring inputFilename, std::vector<float> & texels, int* resxp, int* resyp, int* reszp, float* axisxp, float* axisyp, float* axiszp) {
		
		std::string fn2a = Utils::convertWtoA(inputFilename);
		FILE* ofile = fopen(fn2a.c_str(), "rb");
		if (ofile != NULL) {
			char line[512];
			memset(line,0,512);



			fgets(line, 512, ofile);
			CONSOLE_PRINTF_512("%s", line);
			memset(line, 0, 512);
			fgets(line, 512, ofile);
			CONSOLE_PRINTF_512("%s", line);
			sscanf(line,"%d %d %d",resxp,resyp,reszp);
			float ax, ay, az;
			memset(line, 0, 512);
			fgets(line, 512, ofile);
			CONSOLE_PRINTF_512("%s", line);

			memset(line, 0, 512);
			fgets(line, 512, ofile);
			CONSOLE_PRINTF_512("%s", line);

			sscanf(line, "%f %f %f", &ax, &ay, &az);
			*axisxp = -ax;
			*axisyp = -ay;
			*axiszp = -az;
		

			int endcode=1;
			int bitwidth=32;
			
			memset(line, 0, 512);
			fgets(line, 512, ofile);
			CONSOLE_PRINTF_512("%s", line);
			sscanf(line, "%d %d", &bitwidth,&endcode);

			int numbytes = bitwidth / 8;
			int isbig = 1 - endcode;
			int pisbig = is_big_endian();
		
			CONSOLE_PRINTF_512("%d %d %d %f %f %f\n",*resxp,*resyp,*reszp,*axisxp,*axisyp,*axiszp);

			int Nx = *resxp;
			int Ny = *resyp;
			int Nz = *reszp;
			texels.clear();
			texels.resize(3*Nx*Ny*Nz,0.0f);
		


			for (unsigned int z = 0; z < Nz; z++) {
				for (unsigned int y = 0; y < Ny; y++) {
					for (unsigned int x = 0; x < Nx; x++) {
						unsigned int item_index = x * Ny * Nz + y * Nz + z;
						unsigned int texel_index = item_index * 3;

						uint8_t numberbuffer[4];
						uint8_t tempbuffer[4];

						float sdf_value=0.0;

						fread(numberbuffer,1,4,ofile);

						if (isbig!=pisbig) {
							memcpy(tempbuffer, numberbuffer, 4);
							for (int i = 0; i < 4; i++) {
								numberbuffer[i] = tempbuffer[3 - i];
							}
						


						}

						memcpy(&sdf_value, numberbuffer, 4);
						texels[texel_index+0]=sdf_value;
						texels[texel_index + 1] = sdf_value;
						texels[texel_index + 2] = sdf_value;



					}
				}
			}


		}

		if (ofile != NULL)
			fclose(ofile);
	}

}
