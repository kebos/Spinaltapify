#if defined(___COMPAT_H_SPTY___)
#else
#define ___COMPAT_H_SPTY___
#define EXPORT_LIB_SPTY
#if defined(_WIN32)
	#define PAUSE() system("PAUSE");
#else
        #define T_LIB_DIR
        #define T_LIB_DIR
        #define __declspec  
	#define __stdcall   
	#define PAUSE() printf("Print any key\n"); getchar(); printf("Continuing\n");
#endif

#endif
