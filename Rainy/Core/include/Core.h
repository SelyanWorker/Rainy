#pragma once


//#ifdef RN_PLATFORM_WINDOWS
	#ifdef RN_USING_DLL
		#ifdef RN_BUILD_DLL
			#define RAINY_API _declspec(dllexport)
		#else 
			#define RAINY_API _declspec(dllimport)
		#endif
	#else
		#define RAINY_API
	#endif
//#else 
//	#error Rainy on.y supports on Windows!
//#endif

#ifdef RN_DEBUG
	#define	RN_ASSERT(condition, ...) {if(!(condition)) { RN_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
#else 
	#define RN_ASSERT(condition, ...)  condition
#endif

#define RN_BIND_MEMBER_FUN(fun) std::bind(&fun, this, std::placeholders::_1)
