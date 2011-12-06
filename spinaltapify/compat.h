#if defined(___COMPAT_H_SPTY___)
#else
#define ___COMPAT_H_SPTY___
#define EXPORT_LIB_SPTY
#if defined(WIN32)
        #if defined(EXPORT_LIB_SPTY)
        #define T_LIB_DIR (dllexport)
        #else
        #define T_LIB_DIR (dllimport)
        #endif
	#define PAUSE() system("PAUSE");
#else
        #define T_LIB_DIR
        #define T_LIB_DIR
        #define __declspec  
	#define __stdcall   
	#define PAUSE() printf("Print any key\n"); getchar(); printf("Continuing\n");
#endif

#endif
