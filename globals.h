#ifndef GLOBALS_H_DEFINED
#define GLOBALS_H_DEFINED

/* global constants for runtime */

#define MAX_LINE_LENGTH 64 /* /proc/uptime can't be read with fseek, there a static value is required */
#define UPTIME "/proc/uptime" /* path to uptime provided by the kernel */
#define RUNTIME "/var/cache/runtime" /* path where runtime should store its value */
#define RUNTIME_CACHE "/var/cache/runtime_cache" /* path where runtime should store the cache */
#define CACHE_INTERVAL 3600 /* interval at which runtime should cache its value (in seconds) */
#define VERSION "0.3" /* current version of Runtime */

#endif
