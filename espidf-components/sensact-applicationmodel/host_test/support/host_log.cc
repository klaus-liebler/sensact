#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

void sensact_host_log(char level, const char *tag, const char *format, ...)
{
    static const bool enabled = [] {
        const char *v = std::getenv("SENSACT_TEST_LOG");
        return v != nullptr && std::strcmp(v, "0") != 0;
    }();
    if (!enabled)
        return;
    std::printf("%c (%s) ", level, tag);
    va_list args;
    va_start(args, format);
    std::vprintf(format, args);
    va_end(args);
    std::printf("\n");
}
