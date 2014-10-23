#include "Arduino.h"
#include "WiFlyRN.h"

const char nulchar=0x0;

WiFly::WiFly(){
    _connected=false;
    _commandMode=false;
}

boolean WiFly::begin(Stream * serial, Stream * debug){

    _serial=serial;
    _debug=debug;
    delay(1000);
    _serial->setTimeout(700);

    if(!enterCommandMode()) return false;
    if(!exitCommandMode()) return false;
    return true;
}

void WiFly::sleep(){
    debug("WiFly::sleep time to rest");
    enterCommandMode();
    _commandMode=false;
    _serial->print("sleep\r");
    _serial->flush();
    debug("WiFly::sleep good night");
    findInResponse("\0");
}

void WiFly::debug(const char * m){
    if(_debug!=NULL){
        _debug->println(m);
        delay(100);
    }
}

void WiFly::send(const char * m){
    _serial->print(m);
    debug("WiFly:: str sent to the serial: ");
    debug(m);
    
}

void WiFly::flush(){
    _serial->flush();
}

void WiFly::reboot(){
    debug("WiFly:reboot let's go from the top");
    enterCommandMode();
    _serial->print("reboot\r");
    _commandMode=false;
    findInResponse("*Reboot*\0");
}

boolean WiFly::configTCPConnection(const char * ssid, int auth, const char * passwd, const char * addr, uint16_t port){

    //TODO: Not working, debug
    enterCommandMode();
    _serial->print("set wlan ssid ");
    _serial->print(ssid);
    _serial->print("\r");
    if(!findInResponse("AOK")) return false;
    drain();

    _serial->print("set wlan auth ");
    _serial->print(auth);
    _serial->print("\r");
    if(!findInResponse("AOK")) return false;
    drain();

    _serial->print("set wlan phrase ");
    _serial->print(passwd);
    _serial->print("\r");
    if(!findInResponse("AOK")) return false;
    drain();

    _serial->print("set wlan channel 0\r");
    if(!findInResponse("AOK")) return false;
    drain();

    _serial->print("set wlan join 1\r");
    if(!findInResponse("AOK")) return false;
    drain();

    _serial->print("set ip dhcp 1\r");
    if(!findInResponse("AOK")) return false;
    drain();

    _serial->print("set option format 1\r");
    if(!findInResponse("AOK")) return false;
    drain();

    _serial->print("set ip proto 18\r");
    if(!findInResponse("AOK")) return false;
    drain();

    _serial->print("set uart mode 1\r"); 
    if(!findInResponse("AOK")) return false;
    drain();

    _serial->print("set ip host ");
    _serial->print(addr);
    _serial->print("\r");
    if(!findInResponse("AOK")) return false;
    drain();

    _serial->print("set ip remote ");
    _serial->print(port);
    _serial->print("\r");
    if(!findInResponse("AOK")) return false;
    drain();

    _serial->print("save\r");
    if(!findInResponse("Storing")) return false;
    drain();

    return true;
}

boolean WiFly::factoryReset(){
    enterCommandMode();
    _serial->print("factory RESET\r");
    if(!findInResponse("AOK")) return false;
    drain();
    _serial->print("reboot\r");
    return true;
}

boolean WiFly::openConnection(){

    if(isConnected())
        closeConnection();

    if(!isInCommandMode())
        enterCommandMode();

    _serial->print("open\r");
    delay(100);
    debug("WiFly::openConnection Opening connection... open sent");
    _connected=true;
    if(!findInResponse("*OPEN*")){
        debug("OPEN not found"); 
        isConnected();
        return false;
    }
    debug("WiFly::openConnection Found *OPEN* clause, connection was opened");
    return true;
}
boolean WiFly::openConnection(const char * addr, uint16_t port){

    /**TODO**/
}

boolean WiFly::closeConnection(){
    
    if(!isConnected()) return true;
    
    _serial->print("close\r");
    delay(100);
    debug("WiFly::closeConnection Closing connection... close sent");
    if(!findInResponse("*CLOS*")) return false;
    debug("WiFly::closeConnection Found *CLOS* clause, connection was closed");
    _commandMode=false;
    _connected=false;
    return true;

} 

boolean WiFly::isConnected(){

    if(_serial->available() && _connected){
        debug("WiFly::isConnected available data in serial buffer and connection is open");
        if(findInResponse("CLOS*")){
            debug("WiFly::isConnected Found *CLOS* clause, connection was closed");
            _commandMode=false;
            _connected=false;
        }
    }
    return _connected;
}

boolean WiFly::isInCommandMode(){
    return _commandMode;
}

boolean WiFly::enterCommandMode(){

    if(!isInCommandMode()){
        _serial->print("$$$");
        debug("WiFly::enterCommandMode Entering command mode");
        if(!findInResponse("CMD")){debug("CMD not FOUND"); return false;}
        debug("WiFly::enterCommandMode Found CMD clause, entered command mode");
        _commandMode=true;
        return true;
    }
    return true;
}

boolean WiFly::exitCommandMode(){
    
    if(isInCommandMode()){
        _serial->print("exit\r");
        debug("WiFly::exitCommandMode Exiting command mode");
        delay(500);
        if(!findInResponse("EXIT"))return false;
        debug("WiFly::exitCommandMode Found EXIT clause, exited command mode");
        _commandMode=false;
        return true;
    }
    return false;
}

boolean WiFly::findInResponse(char * str){

    char buffer[32];
    _serial->readBytes(buffer, 31);
    char * occurence = strstr(buffer, str);
    _debug->print("OCCURRENCE: ");
    _debug->println(occurence);
    if(occurence == NULL){
        debug("findInResponse:: match not found");
        return false;
    }
    else{
        debug("findInResponse::match found");
        return true;
    }
/*
    char buffer[32];
    debug("ABOUT TO READ BUFFER");
    readBuffer(buffer, 31);
    debug("BUFFER READ");
    char * occurence = strstr(buffer, str);
    debug("Occurence: ");
    debug(occurence);
    if(occurence == NULL){
        debug("WiFly::findInResponse match not found");
        return false;
    }
    else{
        debug("WiFly::findInResponse match found");
        return true;
    }
    
    */
}

int WiFly::readTimeout(uint16_t timeout){

    int c = -1;
    int deadline = millis()+timeout;
    while(c==-1 && (millis()<deadline)){
        c = _serial->read();
    }
    return c;
}

int WiFly::readBuffer(char * buff, size_t length){

    size_t count = 0;
    *buff='\0';
    while (count < length) {
        debug("WiFly::readBuffer reading a byte");
        int c = readTimeout(1000);
        //_debug->print("Char read : ");
        //_debug->print(c);
        //_debug->print("  ");
        //_debug->println((char)c);
        if (c < 0) break;
        *buff++ = (char)c;
        count++;
    }
    *buff++='\0';
    return count;
}

int WiFly::readBufferUntil(char limit, char * buff, size_t length){

    size_t count = 0;
    *buff='\0';
    while (count < length) {
        int c = readTimeout(1000);
        if (c < 0 || c == (int)limit) break;
        *buff++ = (char)c;
        count++;
    }
    *buff++='\0';
    return count;
}


int WiFly::getResponse(char * buff, size_t length){

    int timeout = 3000;
    int deadline = millis() + timeout;
    int count=0;
    char ch;
    *buff='\0';
    debug("WiFly::getResponse getting response: ");
    return readBufferUntil('*',buff,length);
}

boolean WiFly::drain(uint16_t timeout){

    uint32_t deadline;
    char buffer[32];
    int buffsize = sizeof(buffer)/sizeof(char);

    if(timeout!=NULL){
        timeout = 300;
    }

    deadline = millis()+timeout;
    debug("WiFly::drain starting drain...");
    while(millis()<deadline)
        while(_serial->available()>0) 
            _serial->readBytes(buffer, buffsize);

    debug("WiFly::drain end of draining!");
}

