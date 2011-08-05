#pragma once

extern "C" NTSTATUS ObQueryNameString( IN PVOID Object,
							  OUT POBJECT_NAME_INFORMATION  ObjectNameInfo,
							  IN ULONG  Length,
							  OUT PULONG  ReturnLength); 