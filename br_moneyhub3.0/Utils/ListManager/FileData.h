#pragma once


class CFileData
{

public:

	CFileData();

	void AddFile(LPCTSTR lpszFileName);
	bool CheckFile();

	LPCTSTR GetOneFile() const { ATLASSERT(m_FileNameVec.size() == 1); return m_FileNameVec[0].c_str(); }
	const std::vector<std::wstring>& GetFileList() const { return m_FileNameVec; }

private:

	std::vector<std::wstring> m_FileNameVec;
	bool m_bCheckedFile;
};
