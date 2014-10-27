#ifndef WiFlyRN
#define WiFlyRN
#include "Arduino.h"
#include "Stream.h"


class WiFly{
    public:

        WiFly();
        boolean begin(Stream * serial, Stream * debug=NULL);
        void send(const char * m);
        void sleep();
        int getResponse(char * buff, size_t length);
        void flush();
        void reboot();
        boolean configTCPConnection(const char * ssid, int auth, const char * passwd, const char * addr, uint16_t port);
        boolean factoryReset();
        boolean openConnection();
        boolean openConnection(const char *addr, uint16_t port=80);
        boolean closeConnection();
        boolean isConnected();
        boolean isInCommandMode();
        boolean drain(uint16_t timeout=NULL);

    private:

        void debug(const char * m);
        boolean enterCommandMode();
        boolean exitCommandMode();
        boolean findInResponse(char * str);
        Stream * _serial;
        Stream * _debug;
        boolean _connected;
        boolean _commandMode;

};
        
#endif
