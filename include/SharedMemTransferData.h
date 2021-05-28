#pragma once

#include "CmPoint.h"

#include <iostream>
#include <algorithm>
#include <string>

#include <atlbase.h>
#include <iostream>
#include <Windows.h>

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


template<typename T>
struct SHMEMRegion {
    // Simple class to manage a shared memory region

    std::shared_ptr<windows_shared_memory> shmem;
    std::shared_ptr<mapped_region> region;
    T* data;
	HANDLE semaphore;

    SHMEMRegion(std::string name)
    {
    
        shmem = std::make_shared<windows_shared_memory>(open_or_create, name.c_str(), read_write, sizeof(T));
        region = std::make_shared<mapped_region>(*shmem, read_write);
        
        std::memset(region->get_address(), 0, sizeof(T));
        
        data = reinterpret_cast<T*>(region->get_address());

		// Create the name s semaphore to synchronize access to this structure
		createSemaphore((name + "_SEMPH").c_str(), 1, 1);

    }

	~SHMEMRegion() {
		closeSemaphore();
	}


	// Code for handling windows named semaphores.
	void handleLastError() {
		USES_CONVERSION;
		LPTSTR lpMsgBuf;
		DWORD lastError = GetLastError();
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			lastError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		std::wcout << T2W(lpMsgBuf) << L"\n";
		LocalFree(lpMsgBuf);
	}

	HANDLE createSemaphore(LPCSTR name, int initCount, int maxCount) {
		std::cout << "Creating semaphore " << name << " with counts " << initCount
			<< " of " << maxCount << "...\n";
		HANDLE h = CreateSemaphoreA(NULL, initCount, maxCount, name);
		if (h == NULL) {
			std::cout << "CreateSemaphoreA Error: ";
			handleLastError();
			exit(1);
		}
		std::cout << "Created\n";

		semaphore = h;

		return h;
	}

	HANDLE openSemaphore(LPCSTR name) {
		std::cout << "Opening semaphore " << name << "...\n";
		HANDLE h = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, name);
		if (h == NULL) {
			std::cout << "OpenSemaphoreA Error: ";
			handleLastError();
			exit(1);
		}
		std::cout << "Opened\n";
		
		semaphore = h;

		return h;
	}

	void acquireSemaphore(DWORD timeout_ms) {
		std::cout << "Acquiring...\n";
		DWORD res = WaitForSingleObject(semaphore, timeout_ms);
		if (res == WAIT_OBJECT_0) {
			std::cout << "Acquired\n";
		}
		else if (res == WAIT_TIMEOUT) {
			std::cout << "WaitForSingleObject Timeout\n";
		}
		else if (res == WAIT_FAILED) {
			std::cout << "WaitForSingleObject Error: ";
			handleLastError();
			exit(1);
		}
		else {
			std::cout << "Wait error:" << res << "\n";
			exit(1);
		}
	}

	void releaseSemaphore() {
		std::cout << "Releasing...\n";
		BOOL res = ReleaseSemaphore(semaphore, 1, NULL);
		if (!res) {
			std::cout << "ReleaseSemaphore Error: ";
			handleLastError();
			exit(1);
		}
	}

	int closeSemaphore() {
		std::cout << "Closing...\n";
		BOOL closed = CloseHandle(semaphore);
		if (!closed) {
			std::cout << "CloseHandle Error: ";
			handleLastError();
			exit(1);
		}
		std::cout << "Closed\n";
		return 0;
	}

};


void fillSHMEMData(SHMEMRegion<SHMEMTransferData>& shmem_region, int frame_cnt,
    CmPoint64f& del_rot_cam_vec, double del_rot_error,
    CmPoint64f& del_rot_lab_vec, CmPoint64f& abs_ori_cam_vec,
    CmPoint64f& abs_ori_lab_vec,
    double posx, double posy, double heading, double direction,
    double speed, double intx, double inty, double timestamp,
    int seq_num);

