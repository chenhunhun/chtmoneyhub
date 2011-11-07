#pragma once

#include "atlstr.h"

class CSysModVerify
{
public:
	CSysModVerify(void);
	~CSysModVerify(void);

	bool IsModuleVerified(const CString& filename);
};
