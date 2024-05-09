#include "Server.h"
#include "StringUtils.h"

const char **__argv;
int __argc;

int main(int argc, const char **argv)
{
    __argc = argc;
    __argv = argv;

    if (Server::StaticInit())
    {
        return Server::sInstance->Run();
    }
    else
    {
        return -1;
    }
}