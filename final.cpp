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

// int set_nonblock(int fd)
// {
//     int flags;
// #if defined(O_NONBLOCK)
//     if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
//         flags = 0;
//     return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
// #else
//     flags = 1;
//     return ioctl(fd, FIOBIO, &flags);
// #endif
// }

// void log(int priority, const std::string &message)
// {
//     syslog(priority, "%s", message.c_str());
//     std::cerr << message << std::endl;
// }

// struct ThreadProcArgs
// {
//     int Fd;
//     std::string documentRoot;
// };

// std::vector<std::string> split(const std::string &input, const std::string &delim)
// {
//     std::vector<std::string> result;
//     for (std::string::size_type pos = 0;;)
//     {
//         std::string::size_type index = input.find(delim, pos);
//         if (index == std::string::npos)
//         {
//             result.push_back(input.substr(pos));
//             break;
//         }
//         else
//         {
//             result.push_back(input.substr(pos, index - pos));
//             pos = index + 1;
//         }
//     }
//     return result;
// }

// std::string getResponse(const std::string &method, const std::string &path)
// {
//     const std::string HttpVersion = "HTTP/1.0";
//     std::string statusCode;
//     std::string statusLine;
//     std::string responseBody;
//     if (method == "GET")
//     {
//         std::ifstream in((path).c_str(), std::ifstream::in);
//         if (in.good())
//         {
//             std::stringstream ss;
//             ss << in.rdbuf();
//             in.close();
//             responseBody = ss.str();
//             statusCode = "200 OK";
//         }
//         else
//         {
//             std::cerr << "Cannot read file: " << path << std::endl;
//             statusCode = "404 Not Found";
//         }
//     }
//     else
//     {
//         statusCode = "404 Not Found";
//     }
//     statusLine = HttpVersion + " " + statusCode;
//     std::string responseHeader = "Server: TestServer/1.0 test 1.0";
//     std::string entityHeader = "Allow: GET";
//     entityHeader += "\nContent-Language: en";
//     entityHeader += "\nContent-Length: ";
//     entityHeader += std::to_string(responseBody.size());
//     entityHeader += "\nContent-Type: text/html; charset=ISO-8859-4";
//     std::time_t t = std::time(nullptr);
//     std::tm tm = *std::gmtime(&t);
//     entityHeader += "\nDate: ";
//     std::stringstream ss;
//     ss << std::put_time(&tm, "%c %Z");
//     entityHeader += ss.str();
//     return statusLine + "\n" + responseHeader + "\n" + entityHeader + "\n\n" + responseBody;
// }

// void process(int fd, const std::string &documentRoot)
// {
//     char buffer[1024];
//     memset(buffer, 0, sizeof(buffer));
//     auto size = recv(fd, buffer, sizeof(buffer) / sizeof(char), MSG_NOSIGNAL);
//     if (size == -1)
//     {
//         log(LOG_ALERT, "recv");
//     }
//     else if (size > 0)
//     {
//         std::cout << buffer << std::endl;
//         std::istringstream ss(buffer);
//         char header[1024];
//         memset(header, 0, sizeof(header));
//         ss.getline(header, sizeof(header) / sizeof(char));
//         std::vector<std::string> headerParts = split(header, " ");
//         if (headerParts.size() == 3)
//         {
//             auto query = split(headerParts.at(1), "?");
//             std::string response = getResponse(headerParts.at(0), documentRoot + query.at(0));
//             if (send(fd, response.c_str(), response.size(), MSG_NOSIGNAL) == -1)
//             {
//                 std::cerr << "send" << std::endl;
//             }
//             // std::cerr
//             //     << "Method: " << "\"" << headerParts.at(0) << "\""
//             //     << ", Resource: " << "\"" << headerParts.at(1) << "\""
//             //     << ", Protocol: " << "\"" << headerParts.at(2) << "\""
//             //     << std::endl;
//         }
//     }
//     close(fd);
// }

// void *threadProc(void *arg)
// {
//     ThreadProcArgs *threadProcArgs = static_cast<ThreadProcArgs *>(arg);
//     process(threadProcArgs->Fd, threadProcArgs->documentRoot);
// }

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

    // log(LOG_INFO, "App is started");

    // int sockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // if (sockFd == -1)
    // {
    //     log(LOG_CRIT, "soket");
    //     return -1;
    // }

    // struct sockaddr_in servAddr;
    // memset(&servAddr, 0, sizeof(servAddr));
    // servAddr.sin_family = AF_INET;
    // servAddr.sin_port = htons(port);
    // auto inetAddr = inet_addr(host.c_str());
    // if (inetAddr == INADDR_NONE)
    // {
    //     log(LOG_CRIT, "inet_addr");
    //     return -1;
    // }
    // servAddr.sin_addr.s_addr = inetAddr;
    // try
    // {
    //     if (bind(sockFd, (struct sockaddr *)(&servAddr), sizeof(servAddr)) == -1)
    //     {
    //         throw std::runtime_error("bind");
    //     }
    //     set_nonblock(sockFd);
    //     if (listen(sockFd, SOMAXCONN) == -1)
    //     {
    //         throw std::runtime_error("listen");
    //     }
    //     const int MAX_EVENTS = 10;
    //     struct epoll_event ev, events[MAX_EVENTS];
    //     int epollFd = epoll_create1(0);
    //     if (epollFd == -1)
    //     {
    //         throw std::runtime_error("epoll_create1");
    //     }
    //     ev.data.fd = sockFd;
    //     ev.events = EPOLLIN | EPOLLET;
    //     if (epoll_ctl(epollFd, EPOLL_CTL_ADD, sockFd, &ev) == -1)
    //     {
    //         throw std::runtime_error("epoll_ctl");
    //     }
    //     for (;;)
    //     {
    //         auto nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);
    //         if (nfds == -1)
    //         {
    //             throw std::runtime_error("epoll_wait");
    //         }
    //         for (auto n = 0; n < nfds; ++n)
    //         {
    //             if (events[n].data.fd == sockFd)
    //             {
    //                 struct sockaddr_in cliAddr;
    //                 socklen_t cliAddrLen = sizeof(cliAddr);
    //                 int newSockFd = accept(sockFd, (struct sockaddr *)&cliAddr, (socklen_t *)&cliAddrLen);
    //                 if (newSockFd == -1)
    //                 {
    //                     throw std::runtime_error("accept");
    //                 }
    //                 set_nonblock(newSockFd);
    //                 ev.events = EPOLLIN | EPOLLET;
    //                 ev.data.fd = newSockFd;
    //                 if (epoll_ctl(epollFd, EPOLL_CTL_ADD, newSockFd, &ev) == -1)
    //                 {
    //                     throw std::runtime_error("epoll_ctl");
    //                 }
    //             }
    //             else
    //             {
    //                 pthread_t thread;
    //                 pthread_attr_t threadAttr;
    //                 if (pthread_attr_init(&threadAttr) != 0)
    //                 {
    //                     throw std::runtime_error("pthread_attr_init");
    //                 }
    //                 if (pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED) != 0)
    //                 {
    //                     throw std::runtime_error("pthread_attr_setdetachstate");
    //                 }
    //                 ThreadProcArgs threadProcArgs = {events[n].data.fd, documentRoot};
    //                 if (pthread_create(&thread, &threadAttr, threadProc, &threadProcArgs) != 0)
    //                 {
    //                     if (pthread_attr_destroy(&threadAttr) != 0)
    //                     {
    //                         throw std::runtime_error("pthread_attr_destroy");
    //                     }
    //                     throw std::runtime_error("pthread_create");
    //                 }
    //                 if (pthread_attr_destroy(&threadAttr) != 0)
    //                 {
    //                     throw std::runtime_error("pthread_attr_destroy");
    //                 }
    //             }
    //         }
    //     }
    // }
    // catch (std::exception &ex)
    // {
    //     log(LOG_CRIT, ex.what());
    //     close(sockFd);
    //     return -1;
    // }
    // close(sockFd);

    return 0;
}