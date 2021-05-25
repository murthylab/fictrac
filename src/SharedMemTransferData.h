#ifndef TRANSFER_DATA_H
#define TRANSFER_DATA_H

#include <iostream>
#include <algorithm>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace boost::interprocess; 

struct SHMEMTransferData
{
    int frame_cnt;
    double del_rot_cam_vec[3];
    double del_rot_error;
    double del_rot_lab_vec[3];
    double abs_ori_cam_vec[3];
    double abs_ori_lab_vec[3];
    double posx;
    double posy;
    double heading;
    double direction;
    double speed;
    double intx;
    double inty;
    double timestamp;
    int seq_num;
};

struct SHMEMSignalData
{
    int close;
};

template<class T>
T * setupSHMEMRegion(string name) {

    windows_shared_memory shmem(open_or_create, name, read_write, sizeof(SHMEMTransferData));
    //shared_memory_object shmem(open_or_create, "FicTracStateSHMEM", read_write);
    //shmem.truncate(sizeof(SHMEMTransferData));
    mapped_region region(shmem, read_write);
    std::memset(region.get_address(), 0, sizeof(T));
    T* shared_memory_data = reinterpret_cast<T*>(region.get_address());

    return shared_memory_data
};


#define COPY_VEC3(x) data->x[0] = x[0]; data->x[1] = x[1]; data->x[2] = x[2]; 
void fillSHMEMData(SHMEMTransferData* data, int frame_cnt,
    CmPoint64f & del_rot_cam_vec, double del_rot_error,
    CmPoint64f & del_rot_lab_vec, CmPoint64f & abs_ori_cam_vec,
    CmPoint64f & abs_ori_lab_vec,
    double posx, double posy, double heading, double direction,
    double speed, double intx, double inty, double timestamp,
    int seq_num)
{
    COPY_VEC3(del_rot_cam_vec);
    COPY_VEC3(del_rot_lab_vec);
    COPY_VEC3(abs_ori_cam_vec);
    COPY_VEC3(abs_ori_lab_vec);
    data->frame_cnt = frame_cnt;
    data->del_rot_error = del_rot_error;
    data->posx = posx;
    data->posy = posy;
    data->heading = heading;
    data->direction = direction;
    data->speed = speed;
    data->intx = intx;
    data->inty = inty;
    data->timestamp = timestamp;
    data->seq_num = seq_num;
}

#endif // TRANSFER_DATA_H
