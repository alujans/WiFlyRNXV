#include <WiFlyRN.h>
#include <SoftwareSerial.h>

SoftwareSerial WiFlySerial(2,3);

WiFly wifly;


void setup(){

    Serial.begin(9600);
    WiFlySerial.begin(9600);
    delay(7000);

    if(wifly.begin(&WiFlySerial, &Serial)){
        Serial.println("Se ha inicializado correctamente");
    }else{
        Serial.println("No se ha inicializado correctamente");
    }
}


void loop(){


    char buff[128];

    if(wifly.openConnection()){
        Serial.println("Se ha establecido conexion con el servidor... ");
        wifly.send("POST /users/test.json\n");
        wifly.send("Cache-Control: no-cache\n\n");
        wifly.send("{temperature:algo, humidity:algomas}\n\n");
        wifly.flush();
        //wifly.drain();
        wifly.getResponse(buff);
        Serial.println("Respuesta obtenida: ");
        Serial.println(buff);
        if(wifly.isConnected()){
            wifly.closeConnection();
        }
    }

    delay(10000);
}

