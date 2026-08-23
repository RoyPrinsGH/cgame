#define GRAPHICS_API_OPENGL_33
#define RLGL_IMPLEMENTATION

#include <cstdio>

#define TRACELOG(level, ...)                    \
    do                                          \
    {                                           \
        std::printf("RLGL[%i]: ", (int)(level)); \
        std::printf(__VA_ARGS__);               \
        std::printf("\n");                      \
    } while (0)

#include <rlgl.h>
