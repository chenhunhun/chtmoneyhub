#pragma once

#ifdef _DEBUG
#define	DebugPrint(_x_)		do {	DbgPrint("[%s:%d]:", __FILE__, __LINE__);	\
								DbgPrint _x_;} while (0)
#else
#define DebugPrint(_x_)		
#endif