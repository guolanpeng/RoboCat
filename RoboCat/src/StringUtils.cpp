#include "StringUtils.h"
#include <stdarg.h>

extern const char **__argv;
extern int __argc;

void OutputDebugString(const char *inString)
{
    printf("%s", inString);
}

std::string StringUtils::GetCommandLineArg(int inIndex)
{
    if (inIndex < __argc)
    {   
        //打印命令行参数
        StringUtils::Log("Command line arg %d is %s", inIndex, __argv[inIndex]);
        return std::string(__argv[inIndex]);

    }
    return std::string();
}

std::string StringUtils::Sprintf(const char *inFormat, ...)
{
    // 非线程安全
    static char temp[4096];

    va_list args;
    va_start(args, inFormat);
    vsnprintf(temp, 4096, inFormat, args);
    return std::string(temp);
}

void StringUtils::Log(const char *inFormat, ...)
{
    // 非线程安全
    static char temp[4096];

    va_list args;
    va_start(args, inFormat);
    vsnprintf(temp, 4096, inFormat, args);

    OutputDebugString(temp);
    OutputDebugString("\n");
}