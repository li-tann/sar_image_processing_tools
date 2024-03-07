#include <iostream>
#include <string>
#include <gdal_priv.h>

#include <zlib.h>
#include <minizip/zip.h>
#include <minizip/unzip.h>
#include <tinyxml2.h>

// reference: https://blog.csdn.net/weixin_44120785/article/details/128668476
int main()
{
    // Zip 文件名
    const char *zipfile = "C:/Users/lenovo/Desktop/LT1A_20230321_DaTong_SLC.zip";
    // 需要读取的文件
    const char *file = "LT1A_20230321_SLC_Cut.xml";
    const char * tiffile = "/vsizip/C:/Users/lenovo/Desktop/LT1A_20230321_DaTong_SLC.zip/LT1A_20230321_SLC_Cut.tif";
 
    unzFile uf = unzOpen(zipfile);
    if(uf == NULL)
    {
        std::cerr << "Error opening " << zipfile << std::endl;
        return 1;
    }
 
    // 打开文件
    int err = unzLocateFile(uf, file, 0);
    if(err != UNZ_OK)
    {
        std::cerr << "Error locating " << file << " in " << zipfile << std::endl;
        unzClose(uf);
        return 1;
    }
 
    unz_file_info file_info;
    char filename[512];
    err = unzGetCurrentFileInfo(uf, &file_info, filename, sizeof(filename), NULL, 0, NULL, 0);
    if(err != UNZ_OK)
    {
        std::cerr << "Error getting file info for " << file << std::endl;
        unzClose(uf);
        return 1;
    }
 
    // 打开文件
    err = unzOpenCurrentFile(uf);
    if(err != UNZ_OK)
    {
        std::cerr << "Error opening " << file << std::endl;
        unzClose(uf);
        return 1;
    }
 
    // 读取文件
    std::vector<char> buf(file_info.uncompressed_size);
    int len = unzReadCurrentFile(uf, &buf[0], buf.size());
    if(len < 0)
    {
        std::cerr << "Error reading " << file << std::endl;
        unzCloseCurrentFile(uf);
        unzClose(uf);
        return 1;
    }
 
    // 关闭文件
    unzCloseCurrentFile(uf);
    unzClose(uf);
 
    // // 输出文件
    // std::cout << "Content of " << file << ":(len:100)" << std::endl;
    // // std::cout.write(&buf[0], len);
    // std::cout.write(&buf[0], 100);
    // std::cout <<"..."<< std::endl;

    tinyxml2::XMLDocument doc = new tinyxml2::XMLDocument;
    auto xmlerr = doc.Parse(&buf[0], len);
    if(xmlerr !=tinyxml2::XMLError::XML_SUCCESS){
        std::cerr << "xmlerr !=tinyxml2::XMLError::XML_SUCCESS" << std::endl;
        return 1;
    }

    auto root = doc.RootElement();
    auto mission = root->FirstChildElement("SARParametersofCommonModule")->FirstChildElement("mission")->GetText();
    std::cout<<"xml::mission:"<<mission<<std::endl;


    GDALAllRegister();

    auto ds = (GDALDataset*)GDALOpen(tiffile, GA_ReadOnly);
    if(!ds){
        std::cerr << "ds is nullptr " << std::endl;
        return 1;
    }

    std::cout<<"ds.width & height:"<<ds->GetRasterXSize()<<","<<ds->GetRasterYSize()<<std::endl;
    GDALClose(ds);
 
    return 0;
}
