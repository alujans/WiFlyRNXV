#include "Arduino.h"
#include "WiFlyRN.h"


WiFly::WiFly(){
    _connected=false;
    _commandMode=false;
}

boolean WiFly::begin(Stream * serial, Stream * debug){

    _serial=serial;
    _debug=debug;
    _serial->setTimeout(1500);

    if(!enterCommandMode()) return false;
    if(!exitCommandMode()) return false;
    return true;
}

void WiFly::sleep(){
    enterCommandMode();
    _commandMode=false;
    _serial->print("sleep\r");
    _serial->flush();
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
}
void WiFly::send(double f){
    _serial->print(f);
}

void WiFly::send(int i){
    _serial->print(i);
}

void WiFly::send(const __FlashStringHelper* str){
    _serial->print(str);
}

void WiFly::flush(){
    _serial->flush();
}

void WiFly::reboot(){
    enterCommandMode();
    _serial->print("reboot\r");
    _commandMode=false;
    findInResponse("*Reboot*\0");
}

boolean WiFly::configTCPConnection(const char * ssid, int auth, const char * passwd, const char * addr, uint16_t port){
/*
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
    */
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
    delay(300);
    if(!findInResponse("*OPEN*"))
        return false;
    _connected=true;
    return true;
}

boolean WiFly::openConnection(const char * addr, uint16_t port){
    /**TODO**/
}

void WiFly::clearAndCloseConnection(){
    _serial->print("close\r");
    delay(300);
    _serial->print("exit\r");
    delay(300);
    drain();
    _commandMode=false;
    _connected=false;
}

boolean WiFly::closeConnection(){
    
    if(!isConnected()) return true;
    _serial->print("close\r");
    delay(300);
    if(!findInResponse("CLOS*")) return false;
    _commandMode=false;
    _connected=false;
    return true;

} 

boolean WiFly::isConnected(){

    if(_serial->available() && _connected){
        if(findInResponse("CLOS*")){
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
        _serial->flush();
        delay(400);
        _serial->print("$$$");
        delay(400);
        if(!findInResponse("CMD")){return false;}
        _commandMode=true;
        return true;
    }
    return true;
}

boolean WiFly::exitCommandMode(){
    
    if(isInCommandMode()){
        _serial->print("exit\r");
        delay(300);
        if(!findInResponse("EXIT"))return false;
        _commandMode=false;
        return true;
    }
    return false;
}

boolean WiFly::findInResponse(char * str){

    char buffer[20];
    size_t buffsize = sizeof(buffer)/sizeof(char);
    int count = _serial->readBytes(buffer, buffsize);
    buffer[count] = '\0';
    //_debug->print("Occurence: ");
    //_debug->println(buffer);
    char * occurence = strstr(buffer, str);
    if(occurence == NULL){
        //_debug->print("Not found: ");
        //_debug->println(str);
        return false;
    }
    else{
        return true;
    }
}

int WiFly::getResponse(char * buff, size_t length){
    int count = _serial->readBytesUntil('*',buff,length);
    buff[count] = '\0';
    return count;
}

void WiFly::drain(uint16_t timeout){

    char aux;
    if(!timeout)
        timeout = 500;

    long deadline = millis() + timeout;
    while(millis()<deadline){
        while(_serial->available()){
            aux = _serial->read();
        }
    }
}

