#include "stdafx.h" 
#include "CSVFile.h"   
#include <sstream>   
#include <assert.h>   
 
bool  CSVFile::ReadCSVHead()  
{  
    char strHeadLine[4096];  
	while( 1 )//对于支付宝的csv文件做独立处理，找到流水号等等
	{
		m_CSVFile.getline(strHeadLine, sizeof(strHeadLine)); 
		if (m_CSVFile.eof())  
		{  
			return false;  
		}  
		if(strncmp(strHeadLine,"流水号", 6) == 0)
			break;
	}
  
    RowParse(strHeadLine, sizeof(strHeadLine), m_CSVHead);  
	return true;
}  
  
void  CSVFile::RowParse(const char* strRow, int nSize, ROWVEC& result)  
{  
    result.clear();  
  
    bool    bIsInWord   = false;  
    bool    bIsHaveSpace    = false;  
    std::string strCurWorld;  
  
    for (int i=0; i<nSize; i++)  
    {  
        char ch = strRow[i];  
        if (ch == '/0')  
        {  
            if (i >= 1 && strRow[i-1] == ',')  
            {  
                strCurWorld = ' ';  
            }  
  
            break;  
        }  
  
        bool bIsAdd = true;  
 
        switch (ch)  
        {  
        case ',':  
            {  
                if (!bIsInWord)  
                {  
                    // 一项结束   
  
                    result.push_back(strCurWorld);  
  
                    bIsInWord   = false;  
                    bIsHaveSpace    = false;  
                    strCurWorld = "";  
  
                    bIsAdd  = false;  
                }  
            }  
            break;  
        case '"':  
            {  
                if (!bIsInWord)  
                {  
                    bIsInWord   = true;  
                    bIsHaveSpace    = true;  
  
                    bIsAdd  = false;  
                }  
                else  
                {  
                    if ('"' == strRow[i+1])  
                    {  
                        i++;  
                    }  
                    else if (bIsHaveSpace)  
                    {  
                        bIsInWord   = false;  
  
                        bIsAdd  = false;  
                    }  
                    else  
                    {  
                        assert(0);  
                    }  
  
                }  
            }  
            break;  
        default:  
            //bIsInWord = true;   
            break;  
        };  
 
        if (bIsAdd)  
        {  
            strCurWorld += ch;  
        }  
  
    }  
  
    if (strCurWorld != "")  
    {  
        result.push_back(strCurWorld);  
    }  
  
}  
  
int  CSVFile::FindField(const char* strRow)  
{  
    if (m_nFileState == FILE_STATE_NULL)  
    {  
        return -1;  
    }  
  
    for (ROWVEC::iterator it = m_CSVHead.begin();  
        it != m_CSVHead.end(); it++)  
    {  
        if (*it == strRow)  
        {  
            return int(it - m_CSVHead.begin());  
        }  
    }  
  
    return -1;  
}  
  
  
int  CSVFile::Open(bool bIsRead, const char* strPath, const char* strFilename)  
{  

    m_nFileState    = FILE_STATE_NULL;  
    std::string strFullFileName(strPath);  
    strFullFileName += strFilename;  
  
    m_CSVFile.open(strFullFileName.c_str(), std::ios_base::in);  
    if (!m_CSVFile.is_open())  
    {  
        return 1;  
    }  
  
    if(!ReadCSVHead())
	{
		m_CSVFile.close(); 
		return 2;  
	}
  
    if (bIsRead)  
    {  
        m_nFileState    = FILE_STATE_READ;  
    }  
    else  
    {  
        m_nFileState    = FILE_STATE_WRITE;  
        m_CSVFile.close();  
        m_CSVFile.open(strFullFileName.c_str(), std::ios_base::out);  
        if (!m_CSVFile.is_open())  
       {  
            return 3;  
        }  
  
       m_CSVCurRow = m_CSVHead;   
    }  
 
    return 0;  
}  
  
// 读接口   
bool    CSVFile::CSVReadNextRow()  
{  
    if (m_nFileState != FILE_STATE_READ)  
    {  
        return false;  
    }  
  
    char strHeadLine[4096];  
    m_CSVFile.getline(strHeadLine, sizeof(strHeadLine));  
    if (m_CSVFile.eof())  
    {  
        return false;  
    }
	if(strncmp(strHeadLine, "#", 1) == 0)
		return false;//读到支付宝csv文件的结尾，不再读了，退出
  
    RowParse(strHeadLine, sizeof(strHeadLine), m_CSVCurRow);  
  
   return true;  
}  
  
int replace(std::string& src_str, const std::string& old_str, const std::string& new_str)  
{  
    int count   = 0;  
    int old_str_len = int(old_str.length());  
    int new_str_len = int(new_str.length());  
    int pos = 0;  
    while((pos=int(src_str.find(old_str,pos)))!=std::string::npos)  
    {  
        src_str.replace(pos,old_str_len,new_str);  
        pos+=new_str_len;  
        ++count;  
    }  
    return count;  
}   
  
