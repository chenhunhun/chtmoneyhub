// Updater.h

#pragma once
#include <map>

typedef std::map <std::string , std::string > VERSIONMAP;
extern VERSIONMAP g_oldVersion;
extern VERSIONMAP g_newVersion;

void RunBank();

BOOL CreateDesktopShotCut(tstring strName, tstring strSourcePath);
BOOL DeleteDesktopShotCut(tstring sShotCut);

void  deleteAllReg();
void  feedbackForUpdate();
