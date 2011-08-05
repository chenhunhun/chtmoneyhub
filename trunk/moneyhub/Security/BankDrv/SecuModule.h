#pragma once

//////////////////////////////////////////////////////////////////////////

/**
* 填加安全文件路径
* @param filepath 文件路径。
* @param length 文件路径长度。
* return 返回是否填加成功。
*/
bool AddSecurePath(const WCHAR* filepath, ULONG length);

/**
* 填加安全文件路径
* return 返回是否清除成功
*/
bool ClearSecurePaths();


/**
* 检查文件是否安全
* @param filepath 文件路径。
* return 如果文件是安全的则返回true，否则返回false。
*/
bool IsSecurePath(PUNICODE_STRING filepath);