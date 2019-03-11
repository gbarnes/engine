#include "inc_helpers.h"
#include "../Array.h"
#include "../inc_common.h"

//-----------------------------------------------------------------------------
//  Name : StringExplode (Function)
/// <summary>
/// Explodes a given string into pieces by separator. Stores the results in a 
/// vector. Found at:
/// http://www.infernodevelopment.com/perfect-c-string-explode-split
/// </summary>
//-----------------------------------------------------------------------------
void StringExplode(std::string InString, std::string InSeparator, DS::CArray<std::string>* InResults)
{
	size_t found;
	found = InString.find_first_of(InSeparator);

	while (found != std::string::npos)
	{
		if (found > 0)
		{
			InResults->Push(InString.substr(0, found));
		}

		InString = InString.substr(found + 1);
		found = InString.find_first_of(InSeparator);
	}

	if (InString.length() > 0)
	{
		InResults->Push(InString);
	}
}

//
//
//
