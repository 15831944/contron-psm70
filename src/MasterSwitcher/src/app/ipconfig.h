#ifndef IPCONFIG_H
#define IPCONFIG_H


class Ipconfig
{
public:
    Ipconfig();
    ~Ipconfig();

    bool addIP(char *ethernet, char *ip, char *netmask="", char *gateway="", char *broadcast="");
    bool delIP(char *ethernet, char *ip);
    bool hasIP(char *ip);
};

#endif // IPCONFIG_H
