#include "stdafx.h"
#include "FileData.h"


CFileData::CFileData() : m_bCheckedFile(false) {}


void CFileData::AddFile(LPCTSTR lpszFileName)
{
	m_FileNameVec.push_back(lpszFileName);
}

bool CFileData::CheckFile()
{
	return true;
}
