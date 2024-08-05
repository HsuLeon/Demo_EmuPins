
#include "pch.h"
#include <iostream>
#include "PinEmu.h"

CPinEmu::CPinEmu()
	: m_hMapFile(INVALID_HANDLE_VALUE)
	, m_pBuf(0x0)
	, m_SharedMemorySize(0)
{

}

CPinEmu::~CPinEmu()
{
	this->Close();
}

char* CPinEmu::Create(const wchar_t* sharedMemoryName, int sharedMemorySize)
{
	try
	{
		if (m_pBuf != 0x0) throw std::exception("m_pBuf not 0x0");
		if (sharedMemoryName == 0x0) throw std::exception("null sharedMemoryName");
		if (sharedMemorySize <= 0) throw std::exception("invalid sharedMemorySize");

		// 创建文件映射对象
		HANDLE hMapFile = CreateFileMapping(
			INVALID_HANDLE_VALUE,    // 使用分页文件
			NULL,                    // 默认安全属性
			PAGE_READWRITE,          // 可读写访问
			0,                       // 最大对象大小（高位字）
			sharedMemorySize,        // 最大对象大小（低位字）
			sharedMemoryName);       // 名称

		if (hMapFile == NULL) throw std::exception("Could not create file mapping object: " + GetLastError());

		// 映射视图
		char* buf = (char*)MapViewOfFile(
			hMapFile,                // 句柄
			FILE_MAP_ALL_ACCESS,     // 读写权限
			0,                       // 偏移量（高位字）
			0,                       // 偏移量（低位字）
			sharedMemorySize);       // 映射大小

		if (buf == NULL) {
			CloseHandle(hMapFile);
			throw std::exception("Could not map view of file: " + GetLastError());
		}
		// store 
		m_hMapFile = hMapFile;
		m_pBuf = buf;
		m_SharedMemorySize = sharedMemorySize;
	}
	catch (std::exception ex)
	{

	}
	return m_pBuf;
}

char* CPinEmu::Use(const wchar_t* sharedMemoryName, int sharedMemorySize)
{
	try
	{
		if (m_pBuf != 0x0) throw std::exception("m_pBuf not 0x0");
		if (sharedMemoryName == 0x0) throw std::exception("null sharedMemoryName");
		if (sharedMemorySize <= 0) throw std::exception("invalid sharedMemorySize");

		// 打开文件映射对象
		HANDLE hMapFile = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,     // 读写权限
			FALSE,                   // 继承选项
			sharedMemoryName);       // 名称

		if (hMapFile == NULL) throw std::exception("Could not open file mapping object: " + GetLastError());

		// 映射视图
		char* buf = (char*)MapViewOfFile(
			hMapFile,                // 句柄
			FILE_MAP_ALL_ACCESS,     // 读写权限
			0,                       // 偏移量（高位字）
			0,                       // 偏移量（低位字）
			0);                      // 映射大小（全部）

		if (buf == NULL) {
			CloseHandle(hMapFile);
			throw std::exception("Could not map view of file: " + GetLastError());
		}
		// store 
		m_hMapFile = hMapFile;
		m_pBuf = buf;
		m_SharedMemorySize = sharedMemorySize;
	}
	catch (std::exception ex)
	{

	}
	return m_pBuf;
}

void CPinEmu::Close()
{
	try
	{
		m_SharedMemorySize = 0;
		if (m_pBuf != 0x0)
		{
			UnmapViewOfFile(m_pBuf);
			m_pBuf = 0x0;
		}
		if (m_hMapFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hMapFile);
			m_hMapFile = INVALID_HANDLE_VALUE;
		}
	}
	catch (std::exception ex)
	{

	}
}

bool CPinEmu::DigitalWrite(int pinId, bool bHigh)
{
	unsigned char ch = bHigh ? 0xff : 0x0;
	try
	{
		if (pinId < 0 || pinId >= m_SharedMemorySize) throw std::exception("invalid pinId");
		memcpy(m_pBuf + pinId, &ch, sizeof(char)); // 包括终止符
		return true;
	}
	catch (std::exception ex)
	{
		return false;
	}
}

unsigned char CPinEmu::DigitalRead(int pinId)
{
	unsigned char ch = 0x0;
	if (pinId >= 0 && pinId < m_SharedMemorySize)
	{
		memcpy(&ch, m_pBuf + pinId, sizeof(char));
	}
	return ch;
}