# sar_image_processing_tools

image processing functions for sar

临时性地在终端内添加环境变量, 可以直接使用exe

```shell
#powershell
$env:path += ";/to/gdal_tool/path/bin"

#cmd
set path="%path%;/to/gdal_tool/path/bin"

#linux
export PATH="$PATH:/to/gdal_tool/path/bin"
```

## debug_config_test

测试debug配置参数的模块

## binary_img_to_tif

二进制文件转换为tif，

输入binary, width, height, datatype,  high-low byte trans sign

输出tif

```cpp
    msg +=  " manual:\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]: input binary image filepath,\n"
            " argv[2]: img width,\n"
            " argv[3]: img height,\n"
            " argv[4]: img datatype, support list: int, float, fcomplex ...\n"
            " argv[5]: high-low byte trans, true of false,\n"
            " argv[6]: output tif image filepath,\n";
```

## tif_to_binary

tif转换为二进制文件

输入tif，high-low byte trans sign

输出binary

```cpp
    msg +=  " manual:\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]: input tif filepath, driver is gtiff,\n"
            " argv[2]: high-low byte trans, true of false,\n"
            " argv[3]: output binary image filepath,\n";
```

## complex_transto

复数影像转换为特点几种数据（real_part, imag_part, power, amplitude, phase）

输入tif, method,

输出tif

```cpp
    msg +=  " manual: " EXE_NAME " [cpx filepath] [method] [real filepath]\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]: input, complex filepath, scomplex or fcomplex.\n"
            " argv[2]: input, method, real_part, imag_part, power, amplitude, phase.\n"
            " argv[3]: output, real filepath.";
```

## copy_GeoImage_paste

获取目标数据(target)的范围(start.x/y & width,height), 从参考数据(ref)中提取对应信息, 并更新到目标数据(target)上

输入tgt_tif, ref_tif, tgt.start.x, tgt.start.y, tgt.size.x, tgt.size.y

```cpp
    msg +=  " manual: " EXE_NAME " [target filepath] [ref filepath] [target start x] [target start y] [target width] [target height]\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]: input/output, target filepath.\n"
            " argv[2]: input, reference filepath.\n"
            " argv[3]: input, target start x( pixel). (int)\n"
            " argv[4]: input, target start y( pixel). (int)\n"
            " argv[5]: input, target width. (int)\n"
            " argv[6]: input, target height. (int)\n";
```

## filter_evalutaion

评价滤波结果的功能, 评价标准包含enl, epi, rpn

```cpp
    msg +=  " manual: " EXE_NAME " [input] [params] [output]\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]: input, filtered filepah.\n"
            " argv[2]: input, origin filepath, only epi needed, please print '-' if the method you selected don't need it.\n"
            " argv[3]: method, enl, epi, rpn, ... (single or multi, splited by ',')\n"
            " argv[4]: range, col_start, row_start, width, height (int)\n"
            "          the result will print in cmd and logfile";
```

## rolling_guidance

rolling guidance滤波

```cpp
    msg +=  " manual: " EXE_NAME " [input] [params] [output]\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]: input, flt/fcpx filepath.\n"
            " argv[2]: input, 4 parameters like: step, size, delta_s, delta_r.\n"
            " argv[3]: output, flt/fcpx filepath.";
```

## goldstein

goldstein滤波

```cpp
    msg +=  " manual: " EXE_NAME " [input] [params] [output]\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]: input, flt/fcpx filepath.\n"
            " argv[2]: input, parameter about alpha.\n"
            " argv[3]: input, method, single or omp\n"
            " argv[4]: output, flt/fcpx filepath.";
```

## goldstein_baran

goldstein滤波的优化版，baran滤波

```cpp
    msg +=  " manual: " EXE_NAME " [input] [params] [output]\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]: input, fcpx interf filepath.\n"
            " argv[2]: input, flt, correlation filepath.\n"
            " argv[3]: output, fcpx filtered interf filepath.\n"
            " argv[4]: output, flt alpha filepath.";
```

## goldstein_zhao

goldstein滤波的优化版，zhao滤波

```cpp
    msg +=  " manual: " EXE_NAME " [input] [params] [output]\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]: input, fcpx interf filepath.\n"
            " argv[2]: output, fcpx filtered interf filepath.\n"
            " argv[3]: output, flt alpha filepath.";
```

## interf_spectrum_filter

干涉图的频率滤波，方位向与斜距向，未完成

```cpp
    msg +=  " manual: " EXE_NAME " [input] [params] [output]\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]:  input, master rslc filepath.\n"
            " argv[2]:  input, master par like: freq,bandwith,cnt_freq,inc.\n"
            " argv[3]:  input,  slave rslc filepath.\n"
            " argv[4]:  input,  slave par like: freq,bandwith,cnt_freq,inc.\n"
            " argv[5]:  input, method, az, azline, rg, or rgline.";
```

## pseudo_correlation

计算伪相干系数，不需要主辅影像，输入干涉图即可

```cpp
    msg +=  " manual: " EXE_NAME " [input] [params] [output]\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]: input, fcpx filepath.\n"
            " argv[2]: input, win_size, like 3,5,7,... .\n"
            " argv[3]: output, pesudo correlation filepath(flt).";
```

## interf

很简单的干涉操作，输入主辅影像，生成干涉结果，仅用到了共轭相乘，没有任何滤波操作

```cpp
    msg +=  " manual: " EXE_NAME " [input] [params] [output]\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]:  input, master rslc filepath.\n"
            " argv[2]:  input,  slave rslc filepath.\n"
            " argv[3]:  input, int filepath.";
```

## print_sar_image

sar影像转换为8bit, 4bands格式的数据, 搭配颜色表后输出为png

```cpp
    msg +=  " manual: " EXE_NAME " [sarimage_filepath] [ml] [print_type] [minmax_method] [colormap_filepath] [png_filepath]\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]: input_filepath, supported datatype is short & int & float, scomplex, and fcomplex.\n"
            " argv[2]: multilook.\n"
            " argv[3]: print to (phase or power).\n"
            " argv[4]: minmax method: [auto,0.02(stretch_percentage,0~1)] or [manual,0,1(min & max)].\n"
            " argv[5]: colormap_filepath.\n"
            " argv[6]: print filepath (*.png).\n";
```

## merge_sar_image

融合条纹图与底图

```cpp
    msg +=  " manual:\n" 
            " argv[0]: " EXE_NAME ",\n"
            " argv[1]: input_filepath_up, only supported 4bands, 8bit png image.\n"
            " argv[2]: input_filepath_down, only supported 4bands, 8bit png image.\n"
            " argv[3]: merge method, such as: 'percentage,0.3,0.7', ''.\n"
            " argv[4]: print filepath (*.png).\n";
```
