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

#if DEBUG
#define Assert(Expression) if(!(Expression)) {*(int*)0 = 0;}
#else
#define Assert(Expression)
#endif

#define DEFINES_H
#endif
