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

#define EXE_NAME "phase_simualtion"

using namespace std;
namespace fs = std::filesystem;

string EXE_PLUS_FILENAME(string extention){
    return string(EXE_NAME)+"."+ extention;
}

enum { sim_flat, sim_topo, sim_flat_and_topo};

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

    if(argc < 3){
        msg =   EXE_PLUS_FILENAME("exe\n");
        msg +=  " manual: " EXE_NAME " [input] [params] [output]\n" 
                " argv[0]: " EXE_NAME ",\n"
                " argv[1]: input , height filepath(float).\n"
                " argv[2]: input , scale \n"
                " argv[3]: output, simulation phase filepath(float), warpped.";
        return return_msg(-1,msg);
    }

    return_msg(0,EXE_NAME " start.");

    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

    GDALDataset* ds = (GDALDataset*)GDALOpen(argv[1], GA_ReadOnly);
    if(!ds){
        return return_msg(-2, "ds is nullptr.");
    }
    GDALRasterBand* rb = ds->GetRasterBand(1);

    int ds_width = ds->GetRasterXSize();
    int ds_height= ds->GetRasterYSize();
    GDALDataType datatype = rb->GetRasterDataType();

    if(datatype != GDT_Float32){
        return return_msg(-3, "ds.dataype != float.");
    }

    float* arr = new float[ds_width * ds_height];
    rb->RasterIO(GF_Read, 0, 0, ds_width, ds_height, arr, ds_width, ds_height, datatype, 0, 0);

    GDALClose(ds);

    float lambda = 50;
    float scale = std::stof(argv[2]);

    auto warp = [](float& val){
        if(val > M_PI){
            while(val > M_PI){
                val -= 2 * M_PI;
            }
        }
        else if(val < -M_PI){
            while(val < -M_PI){
                val += 2 * M_PI;
            }
        }
    };

    for(int i=0; i< ds_height * ds_width; i++){
        arr[i] *= scale;
        warp(arr[i]);
    }

    GDALDriver* dv = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset* ds_out = dv->Create(argv[3], ds_width, ds_height, 1, datatype, NULL);
    if(!ds_out){
        delete[] arr;
        return return_msg(-3, "ds_out create failed.");
    }
    GDALRasterBand* rb_out = ds_out->GetRasterBand(1);

    rb_out->RasterIO(GF_Write, 0, 0, ds_width, ds_height, arr, ds_width, ds_height, datatype, 0, 0);
    GDALClose(ds_out);

    delete[] arr;

    return_msg(1, fmt::format("{} spend time :{}s",EXE_NAME, spend_time(start)));

    return return_msg(1, EXE_NAME " end.");
}
