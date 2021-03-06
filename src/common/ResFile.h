// Copyright (C) 2005 GameWG. All rights reserved.
// GAMELOFT PROPRIETARY/CONFIDENTIAL.
//
// Author: Qiu Wei Min

#ifndef __RESFILE_H__
#define __RESFILE_H__

#include "Stream.h"

/// @brief This class provides access to a single resource file packed by "pack" utility.
/// @attention Use CResourceManager::GetResourceAsStream to read resource, avoid using this class directly in game code.
class CResFile
{
public:
	CResFile();
	virtual ~CResFile();
	
	/// @brief Open a resource file.
	///
	/// This resource file must be generated by "pack" utility.
	///	@param[in]	pName	file name of the resource file including path.
	/// @return true if success, or false if failed.
	bool Open(const char *pName);

	/// @brief Close an opened resource file.
	void Close();

	/// @brief Find and open a record by a specified record name.
	/// @attention The name of the resource is case sensitive.
	///	@param[in]	pName	the name of the record.
	/// @return Handle to an opened record, or NULL if failed.
	void* FindRecord(const char *pName);

	/// @brief Get the size of the record data.
	///	@param[in]	hRecord	Handle to the record.
	/// @return Size of the record data.
	unsigned int GetRecordSize(void* hRecord);

	/// @brief Read a whole record into a buffer.
	///	@param[in]	hRecord	Handle to the record.
	///	@param[in]	pBuf	Pointer to a memory buffer to hold the record data.
	///	@param[in]	len	Size of the buffer.
	/// @return The number of record data read in byte, or 0 if the record
	/// size is bigger than the size of the buffer.
	unsigned int ReadRecord(void *hRecord, void *pBuf, unsigned int len);
	
protected:
	class Record{
	public:
		unsigned long m_hash;
		unsigned long m_offset;
	};
	
	unsigned int GetRecordPackedSize(void* hRecord);
	unsigned long ComputeHash(const char *s);

	int m_recordNumber;
	Record *m_records;
	CStream *m_file;

private:
	CResFile& operator=(const CResFile& rhs);
};
#endif