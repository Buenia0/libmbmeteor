// libmbmeteor
// Copyright (C) 2020 BueniaDev
//
// Licensed under the GNU GPL v3 or any later version.
// Refer to the included LICENSE file.

   
#pragma once

#if defined(_WIN32) && !defined(LIBMBMETEOR_STATIC)
    #ifdef LIBMBMETEOR_EXPORTS
        #define LIBMBMETEOR_API __declspec(dllexport)
    #else
        #define LIBMBMETEOR_API __declspec(dllimport)
    #endif // LIBMBGB_EXPORTS

    #define LIBMBMETEOR __cdecl
#else
    #define LIBMBMETEOR_API
    #define LIBMBMETEOR
#endif // _WIN32
