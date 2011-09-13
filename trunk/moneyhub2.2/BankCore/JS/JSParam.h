#include <string>
#pragma once
using namespace std;


typedef struct DownloadStatus
{
	string appId;
	string progress;
	string status;
	string logo;
}DOWNLOADSTATUS,*LPDOWNLOADSTATUS;