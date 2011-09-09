#pragma once


namespace Detours
{
	LONG DetourTransactionBegin();
	LONG DetourUpdateThread(HANDLE hThread);
	LONG DetourAttach(PVOID *ppPointer, PVOID pDetour);
	LONG DetourDetach(PVOID *ppPointer, PVOID pDetour);
	LONG DetourTransactionCommit();
}
