#pragma once

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;

// Check this later!
#pragma warning( disable: 4251 )

#ifdef DAWN_BUILD_DLL 
	#define DAWN_API __declspec(dllexport)
#else
	#define DAWN_API __declspec(dllimport)
#endif

// STL Headers
#include <algorithm>
#include <cassert>
#include <sstream>
#include <chrono>
#include <iostream>
#include <memory>
#include <assert.h>

// The min/max macros conflict with like-named member functions.
// Only use std::min and std::max defined in <algorithm>.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

typedef enum EResult
{
	EResult_OK = 0,		// Success
	EResult_ERROR = 1,		// Common Error
	EResult_CANCELED = 2,		// Action aborted
	EResult_NOT_FOUND = 3,		// Not found
	EResult_WARNING = 4,		// Warning
	EResult_STOP = 5,		// Stop
	EResult_NOT_SUPPORTED = 6,		// Not Supported in case some one tries to start the engine with a too weak hardware
	EResult_ALREADY_EXISTS = 7,			// throw this if something already exists!
	EResult_OUT_OF_MEMORY = 8,
	EResult_TOO_LARGE = 9,
	EResult_INVALID_POINTER = 10,
	EResult_INVALID_CAST = 11,
	EResult_NOT_IMPLEMENTED = 12
} EResult;


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef char c8;
typedef short s16;
typedef int i32;
typedef long i64;


#define TODO(x) __pragma(message("==========> TODO: "_STR(x) " :: " __FILE__ "@"STR(__LINE__)))
#define HAS_FLAG(x,m) (x & m)

inline DAWN_API void Print(std::string InMessage)
{
	std::cout << InMessage.c_str();
}

template<typename T> 
void SafeDelete(T*& a)
{
	if (a != nullptr) 
	{
		delete a;
		a = nullptr;
	}
}


/*
	Taken from http://blog.molecular-matters.com/2013/05/17/adventures-in-data-oriented-design-part-3b-internal-references/
*/
template <size_t N1, size_t N2>
struct SGenericHandle
{
	u32 Index : N1;
	u32 Generation : N2;
};
