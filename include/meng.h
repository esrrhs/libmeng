#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MENG_VERSION "1.0"
#define MENG_VERSION_NUM 100
#define MENG_AUTHOR "esrrhs@163.com"

#define MENG_API extern "C"

struct meng;

typedef void(*meng_main)(meng * m, void * arg);

MENG_API meng * meng_create(meng_main func, size_t stacksize, void * arg);
MENG_API void meng_run(meng * m);
MENG_API bool meng_end(meng * m);
MENG_API void meng_delete(meng * m);
MENG_API void meng_yield(meng * m);
