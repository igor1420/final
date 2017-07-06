#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <fstream>

#include "server.h"

void displayUsage(const std::string &appName)
{
    std::cerr << "Usage: " << appName << " -h host -p port -d dir" << std::endl;
}

int main(int argc, char **argv)
{
    int opt, optCount = 0;
    std::string host, documentRoot;
    unsigned short port;
    while ((opt = getopt(argc, argv, "h:p:d:")) != -1)
    {
        ++optCount;
        switch (opt)
        {
        case 'h':
            host = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 'd':
            documentRoot = optarg;
            break;
        default:
            displayUsage(argv[0]);
            return -1;
        }
    }

    if (optCount != 3)
    {
        displayUsage(argv[0]);
        return -1;
    }

    if (daemon(0, 0) == -1)
    {
        std::cerr << "daemon" << std::endl;
        return -1;
    }

    try
    {
        Server server(host, port, documentRoot);
        server.Start();
    }
    catch (const std::exception &ex)
    {
    }

    return 0;
}
