#include "stdafx.h"

std::string UUIDToString(const UUID& InUUID)
{
	RPC_CSTR uuidToString;
	UuidToStringA(&InUUID, &uuidToString);
	return std::string((char*)uuidToString);
}

UUID StringToUUID(const std::string& InUUIDString)
{
	UUID uuid;
	UuidFromStringA(RPC_CSTR(InUUIDString.c_str()), &uuid);
	return uuid;
}