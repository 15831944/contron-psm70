#ifndef PING_H
#define PING_H


class Ping
{
public:
    Ping();
    ~Ping();

    bool exec(char *ip, int count);
};

#endif // PING_H
