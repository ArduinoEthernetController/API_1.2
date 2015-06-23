//
//  AEC.h
//  
//
//  Created by Massimo Ferrantino on 10.06.15.
//
//

#ifndef ____AEC__
#define ____AEC__

#include "Arduino.h"
#include <EEPROM.h>
#include <Ethernet.h>
#include <SPI.h>

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #define MAXCONTROLS 15
  #define ELEMENTS 6

#else
  #define MAXCONTROLS 0
  #define ELEMENTS 0

#endif

class AEC
{
public:
    void start(int ip[], byte mac[], String name);
    void searchForAction(char& comand, int& value);
    void setHTMLrequest(String webpage);
    void searchForColor(char comand, int& valueR, int& vlaueG, int& valueB, int& alpha);
    void searchForClient();
    void createButton(String name, char comand);
    void createPinButton(String name, int Pin);
    void createPinSlider(String name, int Pin);
    
    void createSlider(String name, char comand, int startValue, int endValue);
    void createFeedback(char flag);
    void createColor(String name, char comand);
    void Feedback(char flag, String message);
    bool receiveTrigger();
    
    void setTypeOfElement(int type);
    String setControlFeedback(String flag, String feedback);
    String createNewArduino(String name);
    
    void newIp();
    void digitalPin();
    void analogPin();
    String getName();
    void writeName();
    
private:
    String _deviceName;
    char _linebuffer[35];
    int _linenumber;
    char _lastComand;
    char _newComand;
    
        int _typeOfElement[MAXCONTROLS] ;
        String _namesOfElements[MAXCONTROLS][6];
        char _comandsOfElements[MAXCONTROLS][6];
        int _valuesOfSlider[MAXCONTROLS][2];
    
        int _iterator;
        int _nameIterator [6];
        int _comandsIterator [6];
        int _valueIterator ;

    

    String _allComands;
    String _allFeedbacks;
    String _returnValue;
    String _ip1, _ip2, _ip3, _ip4, _nameLength;
    String _defaultWebpage;
   
};

#endif /* defined(____AEC__) */
