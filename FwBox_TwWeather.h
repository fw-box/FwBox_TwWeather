//
// Copyright (c) 2020 Fw-Box (https://fw-box.com)
// Author: Hartman Hsieh
//
// Description :
//   
//
// Required Library :
//

#ifndef __FWBOX_TWWEATHER_H__
#define __FWBOX_TWWEATHER_H__

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define OPEN_DATA_PAYLOAD_TYPE_1 1
#define OPEN_DATA_PAYLOAD_TYPE_2 2
#define OPEN_DATA_PAYLOAD_TYPE_3 3

class FwBox_WeatherResult
{
public:
    //
    // Weather string
    //
    String Wx1; // Current weather phenomenon
    String Wx2; // next weather phenomenon
    String Wx3; // next next weather phenomenon
    bool WxResult;
    
    //
    // Temperature
    //
    int T1;
    int T2;
    int T3;
    bool TResult;

    FwBox_WeatherResult();
};

class FwBox_TwWeather
{

public:
    FwBox_TwWeather();

    void begin(String auth);
    void begin(String auth, String location, String dist);
    FwBox_WeatherResult read();
    FwBox_WeatherResult read(int year, int month, int day, int hour, int minute);

    void setAuthorization(String auth);
    void setLocationName(String location);
    void setDistName(String dist);

protected:
    String Authorization;
    String LocationName;
    String DistName;

    void HttpsGet(String Url, String* payload);
    void parse(String* payload, FwBox_WeatherResult* result, int payloadType);
    String encodeUrl(String* str);
    String getDataId(String* str);
};

#endif // __FWBOX_TWWEATHER_H__
