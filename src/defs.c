#include "defs.h"
#include <string.h>

inline int coordToIndex(int x, int y, int width) {
    return y*width + x;
}
inline void indexToCoord(int index, int width, int* x, int* y) {
    *x = index%width;
    *y = index/width;
}

inline float lerp(float v0, float v1, float t) {
    return ((1-t)*v0+t*v1);
}

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#elifdef __APPLE__
#include <mach/mach.h>
#elifdef __linux__
#include <unistd.h>
#include <stdio.h>
#endif

int currentRssBytes(uint64_t *bytes) {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS counters;
    counters.cb = sizeof(counters);

    if(!GetProcessMemoryInfo(GetCurrentProcess(), &counters, sizeof(counters)))
        return 1;
    
    *bytes = (uint64_t)counters.WorkingSetSize;
    return 0;
#elifdef __APPLE__
    struct mach_task_basic_info info;
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;

    if(task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) != KERN_SUCCESS)
        return 1;
    
    *bytes = (uint64_t)info.resident_size;
    return 0;
#elifdef __linux__
    FILE *f = fopen("/proc/self/status", "r");
    if (!f) return 1;

    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        long rss_kib;
        if (sscanf(line, "VmRSS: %ld kB", &rss_kib) == 1) {
            *bytes = (uint64_t)rss_kib*1024;
            found = 1;
            break;
        }
    }

    fclose(f);
    if (!found) {
        fprintf(stderr, "VmRSS not found\n");
        return 1;
    }
    return 0;
#else
    return 1;
#endif
}

float Q_rsqrt(float num) {
    uint32_t i;
    float x2, y;

    x2 = num * 0.5f;
    y  = num;
    
    memcpy(&i, &y, 4);
    i = 0x5f3759df - (i >> 1); // what the heck?
    memcpy(&y, &i, 4);

    // newton approximation
    y = y * (1.5f - (x2 * y * y));

    return y;
}