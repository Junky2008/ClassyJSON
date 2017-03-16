#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x

#define PREFIX_LOG STRINGIZE(__FILE__)":"STRINGIZE(__LINE__)" "

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 7
#endif

#define LOG_EMERG   0
#define LOG_ALERT   1
#define LOG_CRIT    2
#define LOG_ERR     3
#define LOG_WARNING 4
#define LOG_NOTICE  5
#define LOG_INFO    6
#define LOG_DEBUG   7

#ifdef TEST

#define LOG(level, ...)  \
	if (level <= DEBUG_LEVEL) { \
	     if(level==LOG_EMERG)  /* 0 */ { printf("EMERG::"__VA_ARGS__); printf("\n"); } \
	else if(level==LOG_ALERT)  /* 1 */ { printf("ALERT::"__VA_ARGS__); printf("\n"); } \
	else if(level==LOG_CRIT)   /* 2 */ { printf("CRITI::"__VA_ARGS__); printf("\n"); } \
	else if(level==LOG_ERR)    /* 3 */ { printf("ERROR::"__VA_ARGS__); printf("\n"); } \
	else if(level==LOG_WARNING)/* 4 */ { printf("WARNI::"__VA_ARGS__); printf("\n"); } \
	else if(level==LOG_NOTICE) /* 5 */ { printf("NOTIC::"__VA_ARGS__); printf("\n"); } \
	else if(level==LOG_INFO)   /* 6 */ { printf("INFO ::"__VA_ARGS__); printf("\n"); } \
	else if(level==LOG_DEBUG)  /* 7 */ { printf("DEBUG::"__VA_ARGS__); printf("\n"); } \
	} \

#else

#define LOG(level, ...)  \
	if (level <= DEBUG_LEVEL) { \
	     if(level==LOG_EMERG)  /* 0 */ { syslog(LOG_EMERG,  PREFIX_LOG"EMERG::"__VA_ARGS__); } \
	else if(level==LOG_ALERT)  /* 1 */ { syslog(LOG_ALERT,  PREFIX_LOG"ALERT::"__VA_ARGS__); } \
	else if(level==LOG_CRIT)   /* 2 */ { syslog(LOG_CRIT,   PREFIX_LOG"CRITI::"__VA_ARGS__); } \
	else if(level==LOG_ERR)    /* 3 */ { syslog(LOG_ERR,    PREFIX_LOG"ERROR::"__VA_ARGS__); } \
	else if(level==LOG_WARNING)/* 4 */ { syslog(LOG_WARNING,PREFIX_LOG"WARNI::"__VA_ARGS__); } \
	else if(level==LOG_NOTICE) /* 5 */ { syslog(LOG_NOTICE, PREFIX_LOG"NOTIC::"__VA_ARGS__); } \
	else if(level==LOG_INFO)   /* 6 */ { syslog(LOG_INFO,   PREFIX_LOG"INFO ::"__VA_ARGS__); } \
	else if(level==LOG_DEBUG)  /* 7 */ { syslog(LOG_DEBUG,  PREFIX_LOG"DEBUG::"__VA_ARGS__); } \
	} \

#endif
