
#ifndef PINEMU_H
#define PINEMU_H

#include <Windows.h>

class CPinEmu
{
public:
	CPinEmu();
	~CPinEmu();

	char* Create(const wchar_t* sharedMemoryName, int sharedMemorySize);
	char* Use(const wchar_t* sharedMemoryName, int sharedMemorySize);
	void Close();

	bool DigitalWrite(int pinId, bool bHigh);
	unsigned char DigitalRead(int pinId);

protected:
	HANDLE m_hMapFile;
	char* m_pBuf;
	int m_SharedMemorySize;
};

#endif