#ifndef _SERVER_H_
#define _SERVER_H_

#include <string>
#include <vector>

class Server
{
public:
    Server(const std::string &host, unsigned short port, const std::string &documentRoot);
    virtual ~Server();
    void Start();
    void Stop();
    void ProcessRequest(int socket);
private:
    void Log(int priority, const std::string &message)const;
    int SetNonBlock(int fd)const;
    std::string GetResponse(const std::string &method, const std::string &path)const;
    std::vector<std::string> Split(const std::string &input, const std::string &delim)const;
    std::string GetTime()const;

    std::string host;
    unsigned short port;
    std::string documentRoot;
    int masterSocket;
};

#endif
