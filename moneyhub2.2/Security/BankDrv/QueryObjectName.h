#pragma once

/**
* 获取文件路径
* @param Object 文件的FileObject
* return 返回文件路径，失败时返回NULL。
*/
POBJECT_NAME_INFORMATION BkQueryNameString(IN PVOID Object);