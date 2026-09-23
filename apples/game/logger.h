#define PLATFORM_LOG_FUNCTION(name) void name(const char* Format, ...)
typedef PLATFORM_LOG_FUNCTION(log_function_t);
log_function_t* Log;
