#if !defined(DEFINES_H)

#define internal static
#define local_persist static
#define global static

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)

#define PI32 3.14159265359f
#define RAD2DEG(a) ((a) * 180.f / PI32)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte) \
    ((byte) & 0x80 ? '1' : '0'), \
    ((byte) & 0x40 ? '1' : '0'), \
    ((byte) & 0x20 ? '1' : '0'), \
    ((byte) & 0x10 ? '1' : '0'), \
    ((byte) & 0x08 ? '1' : '0'), \
    ((byte) & 0x04 ? '1' : '0'), \
    ((byte) & 0x02 ? '1' : '0'), \
    ((byte) & 0x01 ? '1' : '0')

#if DEBUG
#define Assert(Expression) if(!(Expression)) { fprintf(stderr, "Assertion failed: %s:%d", __FILE__, __LINE__); *(int*)0 = 0;}
#else
#define Assert(Expression)
#endif

#define DEFINES_H
#endif
