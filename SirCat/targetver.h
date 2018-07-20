#pragma once

#if !defined(WINVER) || WINVER != 0x0600
	#define WINVER 0x0600
#endif

#if !defined(_WIN32_WINNT) || _WIN32_WINNT != 0x0600
	#define _WIN32_WINNT 0x0600
#endif

#if !defined(NTDDI_VERSION) || NTDDI_VERSION != 0x06000000
	#define NTDDI_VERSION 0x06000000
#endif

#ifndef STRICT
	#define STRICT 1
#endif

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN 1
#endif

#ifndef NOMINMAX
	#define NOMINMAX 1
#endif

#include <sdkddkver.h>
