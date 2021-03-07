
#include <iostream>
#include <fstream>
#include <cstring>
#include "saveframe.h"

#define BASEDIR "/Users/zhumingwei/project/c++/flvparser/mframes"

int SaveFrame(long long count, AVFrame *frame)
{
    std::string fileyy;
    fileyy = std::string(BASEDIR) + "/" + std::to_string(count) + "/" + "y";
    std::string fileuu = std::string(BASEDIR) + "/" + std::to_string(count) + "/" + "u";
    std::string filevv = std::string(BASEDIR) + "/" + std::to_string(count) + "/" + "v";

    std::ofstream outfile;
    int rec;
    outfile.open(fileyy, std::ios::out);
    if (outfile.is_open())
    {
        int size = frame->linesize[0] * frame->height;
        for (int i = 0; i < size; i++)
        {
            outfile << frame->data[0][i];
        }

        std::cout << "write end y" << std::endl;
        outfile.close();
    }

    outfile.open(fileuu, std::ios::out);
    if (outfile.is_open())
    {
        int size = frame->linesize[1] * frame->height / 2;
        for (int i = 0; i < size; i++)
        {
            outfile << frame->data[1][i];
        }

        std::cout << "write endu" << std::endl;
        outfile.close();
    }
    outfile.open(filevv, std::ios::out);
    if (outfile.is_open())
    {
        int size = frame->linesize[2] * frame->height / 2;
        for (int i = 0; i < size; i++)
        {
            outfile << frame->data[2][i];
        }

        std::cout << "write endvv" << std::endl;
        outfile.close();
    }

    return 0;
}