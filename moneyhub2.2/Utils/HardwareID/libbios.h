#pragma once

//extern DWORD MapBiosToDword(char *bios);
extern bool GetBIOS(char *buf, size_t len, size_t *filled);