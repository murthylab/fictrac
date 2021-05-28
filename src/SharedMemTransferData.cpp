
#include "SharedMemTransferData.h"



#define COPY_VEC3(x) data->x[0] = x[0]; data->x[1] = x[1]; data->x[2] = x[2]; 
void fillSHMEMData(SHMEMRegion<SHMEMTransferData> & shmem_region, int frame_cnt,
    CmPoint64f& del_rot_cam_vec, double del_rot_error,
    CmPoint64f& del_rot_lab_vec, CmPoint64f& abs_ori_cam_vec,
    CmPoint64f& abs_ori_lab_vec,
    double posx, double posy, double heading, double direction,
    double speed, double intx, double inty, double timestamp,
    int seq_num)
{
   
    shmem_region.acquireSemaphore(1000);

    SHMEMTransferData* data = shmem_region.data;
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
    COPY_VEC3(del_rot_cam_vec);
    COPY_VEC3(del_rot_lab_vec);
    COPY_VEC3(abs_ori_cam_vec);
    COPY_VEC3(abs_ori_lab_vec);

    shmem_region.releaseSemaphore();

}
