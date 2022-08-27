#ifndef __DICT_FILE__H__
#define __DICT_FILE__H__
#include "RESRC.h"

#ifdef __cplusplus

#include <io.h>
#include <string>
#include <vector>
#include <fstream>

void getAllFiles(std::string path, std::vector<std::string> &files)
{
    // 文件句柄
    long hFile = 0;
    // 文件信息
    struct _finddata_t fileinfo;

    std::string p;

    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            // 跳过当前目录和父目录
            if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
            {
                // 保存文件的全路径
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0); //寻找下一个，成功返回0，否则-1

        _findclose(hFile);
    }
}
void getAllFiles(std::string path, std::vector<std::string> &files)
{
    //文件句柄
    long hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    std::string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            //比较文件类型是否是文件夹
            if ((fileinfo.attrib & _A_SUBDIR))
            { // 跳过当前目录和父目录
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                {
                    files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                    //递归搜索
                    getAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
                }
            }
            else
            {
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0); //寻找下一个，成功返回0，否则-1
        _findclose(hFile);
    }
}
/*
path: 指定目录
files: 保存结果
fileType: 指定的文件格式，如 .jpg
*/
void getFiles(std::string path, std::vector<std::string> &files, const char *sType)
{
    //文件句柄
    long hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    std::string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            //如果是目录,迭代之
            //如果不是,加入列表
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                {
                    getFiles(p.assign(path).append("\\").append(fileinfo.name), files, sType);
                }
            }
            else
            {
                char *pName = fileinfo.name;
                char *pFind = strstr(pName, sType);
                if (pFind != NULL)
                {
                    files.push_back(p.assign(path).append("\\").append(fileinfo.name));
                }
            }
        } while (_findnext(hFile, &fileinfo) == 0);

        _findclose(hFile);
    }
}
#endif //__cplusplus
#endif //!__DICT_FILE__H__