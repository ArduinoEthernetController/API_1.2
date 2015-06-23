//
//  AEC.cpp
//  
//
//  Created by Massimo Ferrantino on 26.05.15.
//
//

#include "AEC.h"
#include <EEPROM.h>
#include <Ethernet.h>
#include <SPI.h>

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #define MAXCONTROLS 15

#else
  #define MAXCONTROLS 0

#endif

char linebuffer[35];
int linenumber;


void AEC::start(int ip[], byte mac[], String name){
if(MAXCONTROLS == 15){
    _iterator = 0;
    _valueIterator = 0;
    _lastComand = ' ';
     for(int i = 0 ; i < 6; i++){
        _nameIterator[i] = 0;
        _comandsIterator[i] = 0;
    }
}
    _defaultWebpage = "<hr><h1 align='center'><font face='verdana'>Arduino Ethernet Controller</font></h1><hr><font face='verdana'><META http-equiv='refresh' content='5;URL=http://www.massimoferrantino.wordpress.com'>";
    _defaultWebpage += "<p align='center'>MAKE IT AUTOMATED</p><br>";
    _allFeedbacks = "";

    _deviceName = name;
    
    if(EEPROM.read(4) == 0){
        EEPROM.write(0,ip[0]);
        EEPROM.write(1,ip[1]);
        EEPROM.write(2,ip[2]);
        EEPROM.write(3,ip[3]);
        EEPROM.write(4,1);
        
    }
    else{
        _deviceName = getName();
    }
    
    //********************************** Start Web Server*******************************
    IPAddress startIp(EEPROM.read(0),EEPROM.read(1),EEPROM.read(2),EEPROM.read(3));
    EthernetServer server(80);
    Ethernet.begin(mac, startIp);
    server.begin();
    
   //Serial.print("Device at: ");
  // Serial.println(Ethernet.localIP());
    //Serial.println(MAXCONTROLS);
}

void AEC::searchForClient(){
    
    EthernetServer server(80);
    
    EthernetClient client =  server.available();
    if (client) {
        //Serial.println("new client");
        
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                // Serial.write(c);
                if (c == '\n' && currentLineIsBlank) {
                    linebuffer[30];
                    linenumber = 0;
                    
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    client.println("<!DOCTYPE HTML>");
                    client.println(_defaultWebpage);

                 if(MAXCONTROLS == 15){   
                    if(_comandsIterator != 0){
                        for(int j = 0 ; j < 6; j++){
                            _nameIterator[j]--;
                            _comandsIterator[j]--;
                        }
                        _valueIterator--;
                    }
                    if(_iterator > 0){
                        for(int i = 0; i < _iterator; i++){
                            
                            String flag = "";
                            String comand = "";
                            
                            switch (_typeOfElement[i]){
                                case 0:
                                    flag = "NEWBUTTON";
                                    comand = "";
                                    comand += _namesOfElements[_nameIterator[0]][0];
                                    --_nameIterator[0];
                                    comand += "+++";
                                    comand += _comandsOfElements[_comandsIterator[0]][0];
                                    --_comandsIterator[0];
                                    break;
                                case 1:
                                    flag = "NEWSLIDER";
                                    comand = "";
                                    comand += _namesOfElements[_nameIterator[1]][1];
                                    --_nameIterator[1];
                                    comand += "+++";
                                    comand += _comandsOfElements[_comandsIterator[1]][1];
                                    --_comandsIterator[1];
                                    comand += "+++";
                                    comand += String(_valuesOfSlider[_valueIterator][0]);
                                    comand += "+++";
                                    comand += String(_valuesOfSlider[_valueIterator][1]);
                                    --_valueIterator;
                                    break;
                                case 2:
                                    flag = "NEWFEEDBACK";
                                    comand = "";
                                    comand += _comandsOfElements[_comandsIterator[2]][2];
                                    --_comandsIterator[2];
                                    break;
                                case 3:
                                    
                                    flag = "NEWCOLOR";
                                    comand = "";
                                    comand += _namesOfElements[_nameIterator[3]][3];
                                    --_nameIterator[3];
                                    comand += "+++";
                                    comand += _comandsOfElements[_comandsIterator[3]][3];
                                    --_comandsIterator[3];
                                    break;
                                case 4:
                                    flag = "NEWPINBUTTON";
                                    comand = "";
                                    comand += _namesOfElements[_nameIterator[4]][4];
                                    --_nameIterator[4];
                                                                        break;
                                case 5:
                                    flag = "NEWPINSLIDER";
                                    comand = "";
                                    comand += _namesOfElements[_nameIterator[5]][5];
                                    --_nameIterator[5];
                                    
                                    break;
                                default:
                                    break;
                            }
                            _allComands += setControlFeedback(flag ,comand);
                        }
                        _iterator = 0;
                    }
                    client.print(_allComands);
                    }

                    client.print(_allFeedbacks);
                    client.print(createNewArduino(_deviceName));
                    
                    break;
                }
                if (c == '\n') {
                    currentLineIsBlank = true;
                }
                else if (c != '\r') {
                    if(linenumber < 30)
                    {
                        linebuffer[linenumber] = c;
                        linenumber++;
                    }
                    currentLineIsBlank = false;
                }
            }
        }
        delay(1);
        client.stop();
        //Serial.println("client disonnected");
        
    }
    
    if(linebuffer[5] == 'N' && linebuffer[6] == 'E' && linebuffer[7] == 'W' && linebuffer[8] == 'I' && linebuffer[9] == 'P'){
        newIp();
        linebuffer[5] = 0;
    }
    if(linebuffer[5] == 'N' && linebuffer[6] == 'E' && linebuffer[7] == 'W' && linebuffer[8] == 'N'){
        writeName();
        linebuffer[5] = 0;
    }
    if(linebuffer[5] == 'P' && linebuffer[6] == 'I' && linebuffer[7] == 'N' && linebuffer[8] == 'D'){
        digitalPin();
    }
    if(linebuffer[5] == 'P' && linebuffer[6] == 'I' && linebuffer[7] == 'N' && linebuffer[8] == 'A'){
        analogPin();
    }
    if(linebuffer[5] == 'X' && linebuffer[6] == 'X' && linebuffer[7] == 'X'){
        EEPROM.write(4,0);
    }
    
    _allFeedbacks = ""; //delet all feedbacks String for adding new Feedbacks
    return;
}

bool AEC::receiveTrigger(){
    _newComand = linebuffer[5];
    
    if (_lastComand == _newComand) {
        return false;
    }
    else{
        _lastComand = _newComand;
        return true;
    }
}

void AEC::searchForAction(char& comand, int& value){
    comand = linebuffer[5];
    _returnValue = "";
    for( int i = 6 ; i < 10 ; i++){
        _returnValue += linebuffer[i];
    }
    value = _returnValue.toInt();
    return;
}

void AEC::newIp(){
    _ip1 = "";
    _ip1 += linebuffer[10];
    _ip1 += linebuffer[11];
    _ip1 += linebuffer[12];
    _ip2 = "";
    _ip2 += linebuffer[13];
    _ip2 += linebuffer[14];
    _ip2 += linebuffer[15];
    _ip3 = "";
    _ip3 += linebuffer[16];
    _ip3 += linebuffer[17];
    _ip3 += linebuffer[18];
    _ip4 = "";
    _ip4 += linebuffer[19];
    _ip4 += linebuffer[20];
    _ip4 += linebuffer[21];
    
    EEPROM.write(0, _ip1.toInt());
    EEPROM.write(1, _ip2.toInt());
    EEPROM.write(2, _ip3.toInt());
    EEPROM.write(3, _ip4.toInt());
    EEPROM.write(4, 1);
}

String AEC::getName(){
    String readName = "";
    
    for(int i = 0; i < EEPROM.read(5); i++){
        readName += (char)EEPROM.read(6+i);
    }
    
    return readName;
}

void AEC::writeName(){
    _nameLength = "";
    _nameLength += linebuffer[9];
    _nameLength += linebuffer[10];
    
    EEPROM.write(5, _nameLength.toInt());
    
    _deviceName = "";
    for( int i = 0 ; i < _nameLength.toInt() ; i++){
        
        if(linebuffer[11+i] == '~'){
            _deviceName += ' ';
            EEPROM.write(6+i, 32);
        }
        else{
            _deviceName += linebuffer[11 + i];
            EEPROM.write(6+i, linebuffer[11+i]);
        }
    }
    return;
}

void AEC::createButton(String name, char comand){
    
    if (MAXCONTROLS == 0) {
        return;
    }
    
    setTypeOfElement(0);
    
    _namesOfElements[_nameIterator[0]][0] = name;
    _nameIterator[0]++;
    
    _comandsOfElements[_comandsIterator[0]][0] = comand;
    _comandsIterator[0]++;
    //Serial.println("Created Button");
    }


void AEC::createSlider(String name, char comand, int startValue, int endValue){
    if (MAXCONTROLS == 0) {
        return;
    }
    setTypeOfElement(1);
    
    _namesOfElements[_nameIterator[1]][1] = name;
    _nameIterator[1]++;
    
    _comandsOfElements[_comandsIterator[1]][1] = comand;
    _comandsIterator[1]++;
    
    _valuesOfSlider[_valueIterator][0] = startValue;
    _valuesOfSlider[_valueIterator][1] = endValue;
    _valueIterator++;
    //Serial.println("Created Slider");
    
}

void AEC::createPinButton(String name, int Pin){
    if (MAXCONTROLS == 0) {
        return;
    }
    setTypeOfElement(4);
    
    _namesOfElements[_nameIterator[4]][4] = name;
    _namesOfElements[_nameIterator[4]][4] += "+++";
    _namesOfElements[_nameIterator[4]][4] += String(Pin);
    _nameIterator[4]++;
    
        //Serial.println("Created Pin Button");
    }

void AEC::createPinSlider(String name, int Pin){
    if (MAXCONTROLS == 0) {
        return;
    }
    setTypeOfElement(5);
    
    _namesOfElements[_nameIterator[5]][5] = name;
    _namesOfElements[_nameIterator[5]][5] += "+++";
    _namesOfElements[_nameIterator[5]][5] += String(Pin);
    _nameIterator[5]++;
    
    //Serial.println("Created Pin Slider");
    
}

void AEC::createFeedback(char flag){
    if (MAXCONTROLS == 0) {
        return;
    }
    setTypeOfElement(2);
    
    _comandsOfElements[_comandsIterator[2]][2] = flag;
    _comandsIterator[2]++;
    //Serial.println("Created Feedback");
    
}

void AEC::createColor(String name, char comand){
    if (MAXCONTROLS == 0) {
        return;
    }
    setTypeOfElement(3);
    
    _namesOfElements[_nameIterator[3]][3] = name;
    _nameIterator[3]++;
    
    _comandsOfElements[_comandsIterator[3]][3] = comand;
    _comandsIterator[3]++;
    //Serial.println("Created Color");
    
}

void AEC::setTypeOfElement(int type){
    
    _typeOfElement[_iterator] = type;
    _iterator++;
}

String AEC::setControlFeedback(String flag, String feedback){
    String message = "<!--###";
    message += flag;
    message += "###";
    message += feedback;
    message += "###--!>\n";
    return message;
}

String AEC::createNewArduino(String name){
    String dummy = "<!--###ARDUINO###";
    dummy += name;
    dummy += "###--!>";
    
    return dummy;
}

void AEC::digitalPin(){
    String dummy = "";
    dummy += linebuffer[9];
    dummy += linebuffer[10];
    
    pinMode(dummy.toInt(), OUTPUT);
    digitalWrite(dummy.toInt(), linebuffer[11] - '0');
}
void AEC::analogPin(){
    String dummy="";
    dummy += linebuffer[9];
    dummy += linebuffer[10];
    
    pinMode(dummy.toInt(), OUTPUT);
    
    String dummy1="";
    dummy1 += linebuffer[11];
    dummy1 += linebuffer[12];
    dummy1 += linebuffer[13];
    analogWrite(dummy.toInt(), dummy1.toInt());
}

void AEC::setHTMLrequest(String webpage){
    _defaultWebpage = webpage;
}

void AEC::Feedback(char flag, String message){
    _allFeedbacks += "<!--###";
    _allFeedbacks += flag;
    _allFeedbacks += "###";
    _allFeedbacks += message;
    _allFeedbacks += "--!>\n";
}

void AEC::searchForColor(char comand, int& valueR, int& valueG, int& valueB, int& alpha){
    if(linebuffer[5] == comand){
        if (linebuffer[6] >= 48 && linebuffer[6] <= 57) valueR = map(linebuffer[6], 48, 57, 0, 9) * 16;
        if (linebuffer[6] >= 65 && linebuffer[6] <= 70) valueR = map(linebuffer[6], 65, 70, 10, 15) * 16;
        if (linebuffer[6] >= 97 && linebuffer[6] <= 102) valueR = map(linebuffer[6], 97, 102, 10, 15) * 16;
        if (linebuffer[7] >= 48 && linebuffer[7] <= 57) valueR += map(linebuffer[7], 48, 57, 0, 9);
        if (linebuffer[7] >= 65 && linebuffer[7] <= 70) valueR += map(linebuffer[7], 65, 70, 10, 15);
        if (linebuffer[7] >= 97 && linebuffer[7] <= 102) valueR += map(linebuffer[7], 97, 102, 10, 15);
        
        if (linebuffer[8] >= 48 && linebuffer[8] <= 57) valueG = map(linebuffer[8], 48, 57, 0, 9) * 16;
        if (linebuffer[8] >= 65 && linebuffer[8] <= 70) valueG = map(linebuffer[8], 65, 70, 10, 15) * 16;
        if (linebuffer[8] >= 97 && linebuffer[8] <= 102) valueG = map(linebuffer[8], 97, 102, 10, 15) * 16;
        if (linebuffer[9] >= 48 && linebuffer[9] <= 57) valueG += map(linebuffer[9], 48, 57, 0, 9);
        if (linebuffer[9] >= 65 && linebuffer[9] <= 70) valueG += map(linebuffer[9], 65, 70, 10, 15);
        if (linebuffer[9] >= 97 && linebuffer[9] <= 102) valueG += map(linebuffer[9], 97, 102, 10, 15);
        
        if (linebuffer[10] >= 48 && linebuffer[10] <= 57) valueB = map(linebuffer[10], 48, 57, 0, 9) * 16;
        if (linebuffer[10] >= 65 && linebuffer[10] <= 70) valueB = map(linebuffer[10], 65, 70, 10, 15) * 16;
        if (linebuffer[10] >= 97 && linebuffer[10] <= 102) valueB = map(linebuffer[10], 97, 102, 10, 15) * 16;
        if (linebuffer[11] >= 48 && linebuffer[11] <= 57) valueB += map(linebuffer[11], 48, 57, 0, 9);
        if (linebuffer[11] >= 65 && linebuffer[11] <= 70) valueB += map(linebuffer[11], 65, 70, 10, 15);
        if (linebuffer[11] >= 97 && linebuffer[11] <= 102) valueB += map(linebuffer[11], 97, 102, 10, 15);
        
        alpha = ((linebuffer[12] - '0') * 100 + (linebuffer[13] - '0') * 10 + (linebuffer[14] - '0'));
    }
}

