#pragma once

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;

// Check this later!
#pragma warning( disable: 4251 )

#ifdef DAWN_BUILD_DLL 
#define DAWN_API
//#define DAWN_API __declspec(dllexport)
#else
#define DAWN_API
//#define DAWN_API __declspec(dllimport)
#endif

// STL Headers
#include <regex>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <chrono>
#include <iostream>
#include <memory>
#include <assert.h>
#include <array>
#include <set>
#include <functional>
#include <filesystem>
#include <string>
#include <map> // for std::map
#include <mutex> // for std::mutex
#include <vector> // for std::vector
#include <chrono>
#include <Windows.h>
#include <atomic>
#include <Rpc.h> // for uuid only on win32

#pragma comment(lib, "rpcrt4.lib")

#define GLM_GTC_half_float
#include "glm.hpp"
#include "gtx/quaternion.hpp"
#include "spdlog/fmt/fmt.h"

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


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef unsigned long ul64;
typedef float f32;
typedef double f64;

typedef int8_t c8;
typedef int16_t s16;
typedef int32_t i32;
typedef int64_t i64;
typedef long l64;
typedef u64 FileId;

typedef glm::quat quat;
typedef glm::mat4 mat4;
typedef glm::vec4 vec4;
typedef glm::vec3 vec3;
typedef glm::vec2 vec2;

namespace Dawn
{
	using Path = std::filesystem::path;
}

#define _KB(x) (x * 1024)
#define _MB(x) (x * 1024 * 1024)

#define _64KB _KB(64)
#define _1MB _MB(1)
#define _2MB _MB(2)
#define _4MB _MB(4)
#define _8MB _MB(8)
#define _16MB _MB(16)
#define _32MB _MB(32)
#define _64MB _MB(64)
#define _128MB _MB(128)
#define _256MB _MB(256)

#define TODO(x) __pragma(message("todo: "_STR(x) " :: " __FILE__ "@"STR(__LINE__)))
#define HAS_FLAG(x,m) (x & m)

inline DAWN_API void Print(std::string InMessage)
{
	std::cout << InMessage.c_str();
}


#ifdef _DEBUG
#ifndef D_ASSERT
#   define D_ASSERT(Expr, Msg) \
    __D_Assert(#Expr, Expr, __FILE__, __LINE__, Msg)
#endif
#else
#ifndef D_ASSERT
#   define D_ASSERT(Expr, Msg) ;
#endif
#endif

static std::wstring stringToWString(std::string& val)
{
	return std::wstring(val.begin(), val.end());
}

static void __D_Assert(const char* expr_str, bool expr, const char* file, int line, const char* msg)
{
	if (!expr)
	{
		std::string err = fmt::format("Assert failed:\t {0}\nExpected:\t {1}\nSource:\t\t {2}, line {3}\n", msg, expr_str, file, line);
		OutputDebugString(stringToWString(err).c_str());
		abort();
	}
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

// The MIT License(MIT)
// 
// Copyright(c) 2012-2017 Stefan Reinalter
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//template <size_t N1, size_t N2>
struct DAWN_API GenericHandle
{
	u32 Index = 0;
	u32 Generation = 0;
	bool IsValid = false;
};

#define INVALID_HANDLE {0,0, false}

struct DAWN_API AppSettings
{
	HWND Hwnd;
	std::wstring Title;
	u32 Width;
	u32 Height;
	bool IsFullscreen;
	bool UseVsync;
	u32 ColorBits = 32;
	u32 DepthBits = 32;
	u32 AlphaBits = 8;
	bool ShowGDIDebug = false;
};

struct AABBox
{
	vec3 Min;
	vec3 Max;
	vec3 Extents;
};

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T>
using Unique = std::unique_ptr<T>;


extern std::string UUIDToString(const UUID& InUUID);
extern UUID StringToUUID(const std::string& InUUIDString);