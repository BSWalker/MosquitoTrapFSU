/*
MosSharedMem.cpp contains implementations for a shared memory wrapper with exception class extension to be used with the
FSU Mosquito Trap Project with software implemented on the Raspberry Pi platform. This library will allow all the device drivers
and other programs to communicate via shared memory, allowing them to have separate program implemenations. This program is loosely
based upon an example found at:

https://cppcodetips.wordpress.com/2015/02/28/c-wrapper-class-for-shared-memory/

Author: Steven B Walker
Date:   11/20/18
*/

#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <iostream>
#include "MosSharedMem.h"

const std::string lockSemaphoreName = "/semaphoreInit";

MosSharedMem::MosSharedMem(int at_mode) :m_sName("mosMemory"), realTimeData_ptr(NULL), m_iD(-1),
m_SemID(NULL), m_nSize(0)
{
	// initialize struct
	//realTimeData.fanRPM            = 0;
	//realTimeData.totalFanRotations = 0;
	//realTimeData.battVoltage       = 0;
	//realTimeData.CO2pressure       = 0;
	//realTimeData.temperature       = 0;

	// Semaphore open
	m_SemID = sem_open(sLockSemaphoreName.c_str(), O_CREAT, S_IRUSR | S_IWUSR, 1);

	// create and attach data
	if (!isMemCreated()) // note: this test also sets the value of m_iD if true 
	{																		//avoiding subsequent calls to shm_open
		Create(sizeof(realTimeData));
		Attach(at_mode);

		// initialize variables if calling program is first to create shm allocation
		SetFanRPM(0);
		SetTotalFanRev(0);
		SetBattVoltage(0);
		SetTemperature(0);
		SetCO2Pressure(0);
	}
	else Attach(at_mode);
}

////////////////////////////////////////////////////////////////////////////////////////////
//															SHARED MEMORY FUNCTIONS																		//
////////////////////////////////////////////////////////////////////////////////////////////

bool MosSharedMem::isMemCreated()
{
	m_nSize = nSize;
	m_iD = shm_open(m_sName.c_str(), C_READ_WRITE, S_IRWXU | S_IRWXG);
	if (m_iD > 0)
		return true; // memory is pre-existing
	if (m_iD < 0)
	{
		switch (errno)
		{
		case ENOENT:
			return false; // memory is not pre-existing
			break;
		case EACCES:
			throw MSMexception("Permission Exception ");
			break;
		case EINVAL:
			throw MSMexception("Invalid shared memory name passed.");
			break;
		case EMFILE:
			throw MSMexception("The process already has the maximum number of files open.");
			break;
		case ENAMETOOLONG:
			throw MSMexception("The length of name exceeds PATH_MAX.");
			break;
		case ENFILE:
			throw MSMexception("The limit on the total number of files open on the system has been reached");
			break;
		default:
			throw MSMexception("Invalid exception occurred in shared memory creation");
			break;
		}
	}
	return false; // 
}

bool MosSharedMem::Create(size_t nSize, int mode /*= READ_WRITE*/)
{
	m_nSize = nSize;
	m_iD = shm_open(m_sName.c_str(), O_CREAT | mode, S_IRWXU | S_IRWXG);
	if (m_iD < 0)
	{
		switch (errno)
		{
		case EACCES:
			throw MSMexception("Permission Exception ");
			break;
		case EEXIST:
			throw MSMexception("Shared memory object specified by name already exists.");
			break;
		case EINVAL:
			throw MSMexception("Invalid shared memory name passed.");
			break;
		case EMFILE:
			throw MSMexception("The process already has the maximum number of files open.");
			break;
		case ENAMETOOLONG:
			throw MSMexception("The length of name exceeds PATH_MAX.");
			break;
		case ENFILE:
			throw MSMexception("The limit on the total number of files open on the system has been reached");
			break;
		default:
			throw MSMexception("Invalid exception occurred in shared memory creation");
			break;
		}
	}
	/* adjusting mapped file size (make room for the whole segment to map)      --  ftruncate() */
	ftruncate(m_iD, m_nSize);

	return true;
}

bool MosSharedMem::Attach(int mode /*= A_READ | A_WRITE*/)
{
	/* requesting the shared segment    --  mmap() */
	realTimeData_ptr = (SensorData*)mmap(NULL, m_nSize, mode, MAP_SHARED, m_iD, 0);
	if (realTimeData_ptr == NULL)
	{
		throw MSMexception("Exception in attaching the shared memory region");
	}
	return true;
}

bool MosSharedMem::Detach()
{
	munmap(realTimeData_ptr, m_nSize);
}

bool MosSharedMem::Lock()
{
	sem_wait(m_SemID);
}

bool MosSharedMem::UnLock()
{
	sem_post(m_SemID);
}

MosSharedMem::~CSharedMemory()
{
	Clear();
}

void MosSharedMem::Clear()
{
	if (m_iD != -1)
	{
		if (shm_unlink(m_sName.c_str()) < 0)
		{
			perror("shm_unlink");
		}
	}
	/**
	* Semaphore unlink: Remove a named semaphore  from the system.
	*/
	if (m_SemID != NULL)
	{
		/**
		* Semaphore Close: Close a named semaphore
		*/
		if (sem_close(m_SemID) < 0)
		{
			perror("sem_close");
		}
		/**
		* Semaphore unlink: Remove a named semaphore  from the system.
		*/
		if (sem_unlink(lockSemaphoreName.c_str()) < 0)
		{
			perror("sem_unlink");
		}
	}
}

MSMexception::MSMexception(const string &message, bool bSysMsg /*= false*/) throw()
{

}

////////////////////////////////////////////////////////////////////////////////////////////
//																DATA ACCESS FUNCTIONS																		//
////////////////////////////////////////////////////////////////////////////////////////////

void MosSharedMem::SetFanRPM(int rpm)
{
	Lock();
	realTimeData_ptr->fanRPM = rpm;
	UnLock();
	sleep(4);
}

void MosSharedMem::SetTotalFanRev(int tr)
{
	Lock();
	realTimeData_ptr->totalFanRotations = tr;
	UnLock();
	sleep(4);
}

void MosSharedMem::SetBattVoltage(int bv)
{
	Lock();
	realTimeData_ptr->battVoltage = bv;
	UnLock();
	sleep(4);
}

void MosSharedMem::SetTemperature(int tp)
{
	Lock();
	realTimeData_ptr->temperature = tp;
	UnLock();
	sleep(4);
}

void MosSharedMem::SetCO2Pressure(int pr)
{
	Lock();
	realTimeData_ptr->CO2pressure = pr;
	UnLock();
	sleep(4);
}

int MosSharedMem::GetFanRPM(void)
{
	Lock();
	return realTimeData_ptr->fanRPM;
	UnLock();
	sleep(4);
}

int MosSharedMem::GetTotalFanRev(void)
{
	Lock();
	return realTimeData_ptr->totalFanRotations;
	UnLock();
	sleep(4);
}

int MosSharedMem::GetBattVoltage(void)
{
	Lock();
	return realTimeData_ptr->battVoltage;
	UnLock();
	sleep(4);
}

int MosSharedMem::GetTemperature(void)
{
	Lock();
	return realTimeData_ptr->temperature;
	UnLock();
	sleep(4);
}

int MosSharedMem::GetCO2Pressure(void)
{
	Lock();
	realTimeData_ptr->CO2pressure;
	UnLock();
	sleep(4);
}