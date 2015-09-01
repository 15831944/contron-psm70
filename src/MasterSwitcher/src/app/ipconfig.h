#ifndef IPCONFIG_H
#define IPCONFIG_H


class Ipconfig
{
public:
    Ipconfig();
    ~Ipconfig();

    bool addIP(char *ethernet, char *ip, const char *netmask="", const char *gateway="", const char *broadcast="");
    bool delIP(char *ethernet, char *ip);
    bool hasIP(char *ip);
};

#endif // IPCONFIG_H
