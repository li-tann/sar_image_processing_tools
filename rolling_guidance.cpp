#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <filesystem>
#include <complex>
#include <omp.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <fmt/format.h>

#include <gdal_priv.h>

#include "datatype.h"

#define EXE_NAME "rolling_guidance_filter"

using namespace std;
namespace fs = std::filesystem;

string EXE_PLUS_FILENAME(string extention){
    return string(EXE_NAME)+"."+ extention;
}

funcrst rolling_guidance(float* arr_in, int height, int width, float* arr_out, int step, int size, double delta_s, double delta_r);
funcrst rolling_guidance_SSR(int height, int width, complex<float>* arr_ori, complex<float>* arr_j0, int size, double delta_s);
funcrst rolling_guidance_ER(int height, int width, complex<float>* arr_ori, complex<float>* arr_j0, complex<float>* arr_out,  int step, int size, double delta_s, double delta_r);


int main(int argc, char* argv[])
{

    auto start = chrono::system_clock::now();
    string msg;

    auto my_logger = spdlog::basic_logger_mt(EXE_NAME, EXE_PLUS_FILENAME("txt"));

    auto return_msg = [my_logger](int rtn, string msg){
        my_logger->info(msg);
        spdlog::info(msg);
        return rtn;
    };

    if(argc < 4){
        msg =   EXE_PLUS_FILENAME("exe\n");
        msg +=  " manual: " EXE_NAME " [input] [params] [output]\n" 
                " argv[0]: " EXE_NAME ",\n"
                " argv[1]: input, flt/fcpx filepath.\n"
                " argv[2]: input, 4 parameters like: step, size, delta_s, delta_r.\n"
                " argv[3]: output, flt/fcpx filepath.";
        return return_msg(-1,msg);
    }

    return_msg(0,EXE_NAME " start.");

	vector<string> splited_vec;
	strSplit(string(argv[2]), splited_vec, ",");

	if(splited_vec.size() < 4){
		return return_msg(-2,fmt::format("size of argv[2] splited is less than 4 ({}).",splited_vec.size()));
	}

	int step = stoi(splited_vec[0]);
	int size = stoi(splited_vec[1]);
	double delta_s = stod(splited_vec[2]);
	double delta_r = stod(splited_vec[3]);

	GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset* ds = (GDALDataset*)GDALOpen(argv[1], GA_ReadOnly);
    if(!ds){
        return return_msg(-2, "ds is nullptr.");
    }
    GDALRasterBand* rb = ds->GetRasterBand(1);

    int width = ds->GetRasterXSize();
    int height= ds->GetRasterYSize();
    GDALDataType datatype = rb->GetRasterDataType();

	if(datatype != GDT_Float32 && datatype != GDT_CFloat32){
		return return_msg(-2, "datatype is diff with float or fcomplex.");
	}

	
	switch (datatype)
	{
	case GDT_Float32:{
		float* arr_out = new float[width * height];
		float* arr = new float[width * height];
		rb->RasterIO(GF_Read, 0, 0, width, height, arr, width, height, datatype, 0, 0);
		rolling_guidance(arr, height, width, arr_out, step, size, delta_s, delta_r);
		delete[] arr;
		GDALClose(ds);

		GDALDriver* dv = GetGDALDriverManager()->GetDriverByName("GTiff");
		GDALDataset* ds_out = dv->Create(argv[3], width, height, 1, datatype, NULL);
		if(!ds_out){
			delete[] arr_out;
			return return_msg(-3, "ds_out create failed.");
		}
		GDALRasterBand* rb_out = ds_out->GetRasterBand(1);

		rb_out->RasterIO(GF_Write, 0, 0, width, height, arr_out, width, height, datatype, 0, 0);

		delete[] arr_out;
		GDALClose(ds_out);
	}break;
	case GDT_CFloat32:{
		complex<float>* arr = new complex<float>[width * height];
		complex<float>* arr_jt0 = new complex<float>[width * height];
		rb->RasterIO(GF_Read, 0, 0, width, height, arr, width, height, datatype, 0, 0);
		cout<<"1"<<endl;
		rolling_guidance_SSR(height, width, arr, arr_jt0, size, delta_s);
		cout<<arr_jt0[1].real();
		cout<<"2"<<endl;
		complex<float>* arr_out = new complex<float>[width * height];
		rolling_guidance_ER(height, width, arr, arr_jt0, arr_out, step, size, delta_s, delta_r);
		cout<<"3"<<endl;
		delete[] arr;
		delete[] arr_jt0;
		GDALClose(ds);

		GDALDriver* dv = GetGDALDriverManager()->GetDriverByName("GTiff");
		GDALDataset* ds_out = dv->Create(argv[3], width, height, 1, datatype, NULL);
		if(!ds_out){
			delete[] arr_out;
			return return_msg(-3, "ds_out create failed.");
		}
		GDALRasterBand* rb_out = ds_out->GetRasterBand(1);

		rb_out->RasterIO(GF_Write, 0, 0, width, height, arr_out, width, height, datatype, 0, 0);

		delete[] arr_out;
		GDALClose(ds_out);
	}break;
	default:
		return return_msg(-1,"unknown datatype.");
		break;
	}

	return_msg(1, fmt::format("{} spend time :{}s",EXE_NAME, spend_time(start)));
    return return_msg(1, EXE_NAME " end.");
}


funcrst rolling_guidance(float* arr_in, int height, int width, float* arr_out, int step, int size, double delta_s, double delta_r)
{
	auto is_point_in_image = [width, height]( int i, int j){
		if(i < 0 || j < 0 || i > height - 1 || j > width - 1)
			return false;
		return true;
	};

	//Step1: Small Structure Removal
	float* jt0 = new float[height * width];

#pragma omp parallel for
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){

			float G = 0, K = 0;
			for (int m = -size; m <= size; m++){
				for (int n = -size; n <= size; n++){
					if(!is_point_in_image(i+m, j+n))
						continue;
					float temp = (float)exp(-(m*m+n*n) / 2. / pow(delta_s, 2));
					K += temp;
					G += temp * arr_in[(i+m)*width + (j+n)];
				}
			}
			jt0[i * width + j] = (K == 0. ? 0 : G / K);
		}
	}

	//Step2: Edge Recovery (iteration)
	int iter_num = 0;
	do{
		/// init jt1
		float* jt1 = new float[height * width];

#pragma omp parallel for
		for(int i = 0; i < height; i++){
			for(int j = 0; j < width; j++){

				float J = 0, K = 0;
				for (int m = -size; m <= size; m++){
					for (int n = -size; n <= size; n++){
						if(!is_point_in_image(i+m, j+n))
							continue;
						float temp = (float)exp(-(m*m+n*n) / 2. / pow(delta_s, 2) - pow(jt0[(i+m)*width+(j+n)] - jt0[i*width+j], 2) / (2 * pow(delta_r, 2)) );
						K += temp;
						J += temp * arr_in[(i+m)*width + (j+n)];
					}
				}
				
				jt1[i * width + j] = (K == 0. ? 0 : J / K);
			}
		}

		for(size_t i=0; i < height*width; i++){
			jt0[i] = jt1[i];
		}
		delete[] jt1;

		iter_num++;
	} while (iter_num < step);

	if(arr_out == nullptr){
		arr_out = new float[height * width];
	}
	else if(dynamic_array_size(arr_out) != height * width){
		delete[] arr_out;
		arr_out = new float[height * width];
	}

	for(size_t i=0; i < height * width; i++){
		arr_out[i] = jt0[i];
	}

	delete[] jt0;
	
	return funcrst(true, "rolling_guidance finished.");
}

/// SSR: Small Structure Removal
funcrst rolling_guidance_SSR(
	int height, int width, 
	complex<float>* arr_ori, 
	complex<float>* arr_j0, 
	int size, 
	double delta_s)
{
	auto is_point_in_image = [width, height]( int i, int j)->bool{
		if(i < 0 || j < 0 || i > height - 1 || j > width - 1)
			return false;
		return true;
	};

	//Step1: Small Structure Removal
	if(arr_j0 == nullptr){
		arr_j0 = new complex<float>[height * width];
	}else if(dynamic_array_size(arr_j0) != width * height){
		delete[] arr_j0;
		arr_j0 = new complex<float>[height * width];
	}

#pragma omp parallel for
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){

			complex<float> G(0,0);
			float K = 0;
			for (int m = -size; m <= size; m++){
				for (int n = -size; n <= size; n++){
					if(!is_point_in_image(i+m, j+n))
						continue;
					float temp = (float)exp(-(m*m+n*n) / 2. / pow(delta_s, 2));
					K += temp;
					G += temp * arr_ori[(i+m)*width + (j+n)];
				}
			}
			arr_j0[i * width + j] = (K == 0 ? 0 : G / K);
		}
	}

	return funcrst(true, "rolling_guidance_SSR finished.");
}

/// ER: Edge Recovery
funcrst rolling_guidance_ER(
	int height, int width, 
	complex<float>* arr_ori, complex<float>* arr_j0, 
	complex<float>* arr_out,  
	int step, int size, 
	double delta_s, double delta_r)
{
	auto is_point_in_image = [width, height]( int i, int j)->bool{
		if(i < 0 || j < 0 || i > height - 1 || j > width - 1)
			return false;
		return true;
	};

	auto cpx_phase = [](complex<float> cpx)->float{
		return atan2(cpx.imag(), cpx.real());
	};

	//Step2: Edge Recovery Iteration
	int iter_num = 0;
	complex<float>* jt0 = new complex<float>[height * width];
	for(int i=0; i< height * width; i++){
		jt0[i] = arr_j0[i];
	}
	
	while(iter_num < step){
		complex<float>* jt1 = new complex<float>[height * width];

// #pragma omp parallel for
		for(int i = 0; i < height; i++){
			for(int j = 0; j < width; j++){

				complex<float> J(0,0);
				float K = 0;
				for (int m = -size; m <= size; m++){
					for (int n = -size; n <= size; n++){
						if(!is_point_in_image(i+m, j+n))
							continue;
							
						float temp = (float)exp(-(m*m+n*n) / 2. / pow(delta_s, 2) - pow(cpx_phase(jt0[(i+m)*width+(j+n)]) - cpx_phase(jt0[i*width+j]), 2) / (2 * pow(delta_r, 2)) );
						K += temp;
						J += temp * arr_ori[(i+m)*width + (j+n)];
					}
				}
				
				jt1[i * width + j] = (K == 0. ? 0 : J / K);
			}
		}

		for(size_t i=0; i < height*width; i++){
			jt0[i] = jt1[i];
		}
		delete[] jt1;

		iter_num++;
	};

	for(int i=0; i< width * height; i++){
		arr_out[i] = jt0[i];
	}

	return funcrst(true, "rolling_guidance_ER finished.");
}