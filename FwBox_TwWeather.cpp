//
// Copyright (c) 2020 Fw-Box (https://fw-box.com)
// Author: Hartman Hsieh
//
// Description :
//   
//
// Required Library :
//

#include "FwBoxLiteJson.h"
#include "FwBox_TwWeather.h"

using namespace FwBox; 


FwBox_WeatherResult::FwBox_WeatherResult()
{
  FwBox_WeatherResult::Wx1 = "";
  FwBox_WeatherResult::Wx2 = "";
  FwBox_WeatherResult::Wx3 = "";
  FwBox_WeatherResult::WxResult = false;
  FwBox_WeatherResult::T1 = 0;
  FwBox_WeatherResult::T2 = 0;
  FwBox_WeatherResult::T3 = 0;
  FwBox_WeatherResult::TResult = false;

}

FwBox_TwWeather::FwBox_TwWeather()
{
  FwBox_TwWeather::Authorization = "";
}

void FwBox_TwWeather::begin(String auth)
{
  FwBox_TwWeather::begin(auth, "新北市", "新莊區");
}

void FwBox_TwWeather::begin(String auth, String location, String dist)
{
    FwBox_TwWeather::Authorization = auth;
    FwBox_TwWeather::LocationName = location;
    FwBox_TwWeather::DistName = dist;
}

FwBox_WeatherResult FwBox_TwWeather::read()
{
    return FwBox_TwWeather::read(0, 0, 0, 0, 0);
}

FwBox_WeatherResult FwBox_TwWeather::read(int year, int month, int day, int hour, int minute)
{
    FwBox_WeatherResult result;

    if(FwBox_TwWeather::LocationName.length() <= 0) {
        return result;
    }

    String url = "https://opendata.cwb.gov.tw/api/v1/rest/datastore/";
    
    //
    // 僅縣市
    //
    if(FwBox_TwWeather::DistName.length() <= 0 || year < 2020 || year > 2100 || month == 0 || day == 0) {
        url += "F-C0032-001?Authorization=";
        url += FwBox_TwWeather::Authorization;
        url += "&format=JSON&locationName=";
        url += FwBox_TwWeather::encodeUrl(&(FwBox_TwWeather::LocationName));
        url += "&elementName=Wx,MinT,MaxT";

        //Serial.println(url);
        
        //
        // Send the request and parse the response.
        //
        String payload = "";
        int payload_type = OPEN_DATA_PAYLOAD_TYPE_1;
        HttpsGet(url, &payload);
        
        FwBox_TwWeather::parse(&payload, &result, payload_type);
        
        return result;
    }
    //
    // 縣市 + 區
    //
    else {
        String data_id = FwBox_TwWeather::getDataId(&(FwBox_TwWeather::LocationName));
        if(data_id.length() > 0) {
            char buff[20];
            memset(&(buff[0]), 0, 20);
            // 2020-12-23T24:00:00
            sprintf(buff, "%d-%02d-%02dT24:00:00", year, month, day);
            String str_time_to = buff;

            url += data_id;
            url += "?Authorization=";
            url += FwBox_TwWeather::Authorization;
            url += "&format=JSON&locationName=";
            url += FwBox_TwWeather::encodeUrl(&(FwBox_TwWeather::DistName));
            //url += "&timeTo=";
            //url += FwBox_TwWeather::encodeUrl(&str_time_to);
            
            //
            // Send the request and parse the response.
            //
            String payload = "";
            String new_url = url + "&elementName=Wx";
            Serial.println(new_url);
            int payload_type = OPEN_DATA_PAYLOAD_TYPE_2;
            HttpsGet(new_url, &payload);
            FwBox_TwWeather::parse(&payload, &result, payload_type);

            payload = "";
            new_url = url + "&elementName=T";
            //Serial.println(new_url);
            payload_type = OPEN_DATA_PAYLOAD_TYPE_3;
            HttpsGet(new_url, &payload);
            FwBox_TwWeather::parse(&payload, &result, payload_type);

            return result;
        }
        else {
            return result; // Error, return empty object.
        }
    }
}

//void FwBox_TwWeather::HttpsGet(String Url, FwBox_WeatherResult* result)
void FwBox_TwWeather::HttpsGet(String Url, String* payload)
{
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

  client->setInsecure();

  HTTPClient https;

  //Serial.print("[HTTPS] begin...\n");
  if (https.begin(*client, Url)) {  // HTTPS

    //Serial.print("[HTTPS] GET...\n");

    // start connection and send HTTP header
    int httpCode = https.GET();
    (*payload) = https.getString();

    //Serial.println("end...");

    https.end();
  }
  else {
    //Serial.printf("[HTTPS] Unable to connect\n");
  }
}

void FwBox_TwWeather::parse(String* payload, FwBox_WeatherResult* result, int payloadType)
{
    FwBoxLiteJson lite_json;
    int pos = 0;

    //Serial.print("payloadType = ");
    //Serial.println(payloadType);

    if (payloadType == OPEN_DATA_PAYLOAD_TYPE_1) {
        int elements_size = 3;
        for(int vi = 0; vi < elements_size; vi++) {
            String str_val = "";
            if (lite_json.getString(payload, &pos, "elementName", &str_val) == 0) {
                if (str_val.equals("Wx") == true) {
                    //Serial.print("pos = ");
                    //Serial.println(pos);
                    //Serial.print("str_val (Wx) = ");
                    //Serial.println(str_val);
                    String parameter_name = "";
                    lite_json.getString(payload, &pos, "parameterName", &parameter_name);
                    //Serial.println(parameter_name);
                    result->Wx1 = parameter_name;
                    parameter_name = "";
                    lite_json.getString(payload, &pos, "parameterName", &parameter_name);
                    //Serial.println(parameter_name);
                    result->Wx2 = parameter_name;
                    parameter_name = "";
                    lite_json.getString(payload, &pos, "parameterName", &parameter_name);
                    //Serial.println(parameter_name);
                    result->Wx3 = parameter_name;
                    result->WxResult = true; // Success
                }
                else if (str_val.equals("MinT") == true) {
                    //Serial.print("pos = ");
                    //Serial.println(pos);
                    //Serial.print("str_val (MinT) = ");
                    //Serial.println(str_val);
                    String parameter_name = "";
                    lite_json.getString(payload, &pos, "parameterName", &parameter_name);
                    //Serial.print("MinT=");Serial.println(parameter_name);
                    result->T1 = parameter_name.toInt();
    
                    parameter_name = "";
                    lite_json.getString(payload, &pos, "parameterName", &parameter_name);
                    //Serial.print("MinT=");Serial.println(parameter_name);
                    result->T2 = parameter_name.toInt();
    
                    parameter_name = "";
                    lite_json.getString(payload, &pos, "parameterName", &parameter_name);
                    //Serial.print("MinT=");Serial.println(parameter_name);
                    result->T3 = parameter_name.toInt();
    
                    result->TResult = true; // Success
                }
                else if (str_val.equals("MaxT") == true) {
                    //Serial.print("pos = ");
                    //Serial.println(pos);
                    //Serial.print("str_val (MaxT) = ");
                    //Serial.println(str_val);
                    String parameter_name = "";
                    lite_json.getString(payload, &pos, "parameterName", &parameter_name);
                    //Serial.print("MaxT=");Serial.println(parameter_name);
                    if(result->T1 > 0)
                        result->T1 = (result->T1 + parameter_name.toInt()) / 2; // Calculate the average value
                    else
                        result->T1 = parameter_name.toInt();
    
                    parameter_name = "";
                    lite_json.getString(payload, &pos, "parameterName", &parameter_name);
                    //Serial.print("MaxT=");Serial.println(parameter_name);
                    if(result->T2 > 0)
                        result->T2 = (result->T2 + parameter_name.toInt()) / 2; // Calculate the average value
                    else
                        result->T2 = parameter_name.toInt();
    
                    parameter_name = "";
                    lite_json.getString(payload, &pos, "parameterName", &parameter_name);
                    //Serial.print("MaxT=");Serial.println(parameter_name);
                    if(result->T3 > 0)
                        result->T3 = (result->T3 + parameter_name.toInt()) / 2; // Calculate the average value
                    else
                        result->T3 = parameter_name.toInt();
    
                    result->TResult = true; // Success
                }
            }
        }
    }
    else if (payloadType == OPEN_DATA_PAYLOAD_TYPE_2) {
        //Serial.println("OPEN_DATA_PAYLOAD_TYPE_2:01");
        //Serial.println(*payload);
        for (int vi = 0; vi < 6; vi++) {
            String str_val = "";
            if (lite_json.getString(payload, &pos, "elementName", &str_val) == 0) {
                //Serial.println("OPEN_DATA_PAYLOAD_TYPE_2:02");
                if (str_val.equals("Wx") == true) {
                    //Serial.print("pos = ");
                    //Serial.println(pos);
                    //Serial.print("str_val (Wx) = ");
                    //Serial.println(str_val);
                    String parameter_name = "";
                    lite_json.getString(payload, &pos, "value", &parameter_name);
                    //Serial.println(parameter_name);
                    result->Wx1 = parameter_name;
                    parameter_name = "";
                    lite_json.getString(payload, &pos, "value", &parameter_name);

                    parameter_name = "";
                    lite_json.getString(payload, &pos, "value", &parameter_name);
                    //Serial.println(parameter_name);
                    result->Wx2 = parameter_name;
                    parameter_name = "";
                    lite_json.getString(payload, &pos, "value", &parameter_name);

                    parameter_name = "";
                    lite_json.getString(payload, &pos, "value", &parameter_name);
                    //Serial.println(parameter_name);
                    result->Wx3 = parameter_name;
                    parameter_name = "";
                    lite_json.getString(payload, &pos, "value", &parameter_name);

                    result->WxResult = true; // Success
                }
            }
        }
    }
    else if (payloadType == OPEN_DATA_PAYLOAD_TYPE_3) {
        //Serial.println("OPEN_DATA_PAYLOAD_TYPE_3:01");
        for (int vi = 0; vi < 6; vi++) {
            String str_val = "";
            if (lite_json.getString(payload, &pos, "elementName", &str_val) == 0) {
                //Serial.println("OPEN_DATA_PAYLOAD_TYPE_3:02");
                if (str_val.equals("T") == true) {
                    //Serial.print("pos = ");
                    //Serial.println(pos);
                    //Serial.print("str_val (T) = ");
                    //Serial.println(str_val);
                    String parameter_name = "";
                    lite_json.getString(payload, &pos, "value", &parameter_name);
                    //Serial.print("T=");Serial.println(parameter_name);
                    result->T1 = parameter_name.toInt();

                    parameter_name = "";
                    lite_json.getString(payload, &pos, "value", &parameter_name);
                    //Serial.print("T=");Serial.println(parameter_name);
                    result->T2 = parameter_name.toInt();

                    parameter_name = "";
                    lite_json.getString(payload, &pos, "value", &parameter_name);
                    //Serial.print("T=");Serial.println(parameter_name);
                    result->T3 = parameter_name.toInt();

                    result->TResult = true; // Success
                }
            }
        }
    }
}

//
// Example : 
// Transform "新北市" to "%E6%96%B0%E5%8C%97%E5%B8%82"
//
String FwBox_TwWeather::encodeUrl(String* str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str->length(); i++) {
      c=str->charAt(i);
      if (c == ' '){
        encodedString+= '+';
      }
      else if (isalnum(c)) {
        encodedString+=c;
      }
      else {
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}

String FwBox_TwWeather::getDataId(String* str)
{
/*
F-D0047-001(宜蘭縣), 
F-D0047-005(桃園市), 
F-D0047-009(新竹縣), 
F-D0047-013(苗栗縣), 
F-D0047-017(彰化縣), 
F-D0047-021(南投縣), 
F-D0047-025(雲林縣), 
F-D0047-029(嘉義縣), 
F-D0047-033(屏東縣), 
F-D0047-037(臺東縣), 
F-D0047-041(花蓮縣), 
F-D0047-045(澎湖縣), 
F-D0047-049(基隆市), 
F-D0047-053(新竹市), 
F-D0047-057(嘉義市), 
F-D0047-061(臺北市), 
F-D0047-065(高雄市), 
F-D0047-069(新北市), 
F-D0047-073(臺中市), 
F-D0047-077(臺南市), 
F-D0047-081(連江縣), 
F-D0047-085(金門縣)
*/
    if(str->equals("宜蘭縣") == true) {
        return "F-D0047-001";
    }
    else if(str->equals("桃園市") == true) {
        return "F-D0047-005";
    }
    else if(str->equals("新竹縣") == true) {
        return "F-D0047-009";
    }
    else if(str->equals("苗栗縣") == true) {
        return "F-D0047-013";
    }
    else if(str->equals("彰化縣") == true) {
        return "F-D0047-017";
    }
    else if(str->equals("南投縣") == true) {
        return "F-D0047-021";
    }
    else if(str->equals("雲林縣") == true) {
        return "F-D0047-025";
    }
    else if(str->equals("嘉義縣") == true) {
        return "F-D0047-029";
    }
    else if(str->equals("屏東縣") == true) {
        return "F-D0047-033";
    }
    else if(str->equals("臺東縣") == true || str->equals("台東縣") == true) {
        return "F-D0047-037";
    }
    else if(str->equals("花蓮縣") == true) {
        return "F-D0047-041";
    }
    else if(str->equals("澎湖縣") == true) {
        return "F-D0047-045";
    }
    else if(str->equals("基隆市") == true) {
        return "F-D0047-049";
    }
    else if(str->equals("新竹市") == true) {
        return "F-D0047-053";
    }
    else if(str->equals("嘉義市") == true) {
        return "F-D0047-057";
    }
    else if(str->equals("臺北市") == true || str->equals("台北市") == true) {
        return "F-D0047-061";
    }
    else if(str->equals("高雄市") == true) {
        return "F-D0047-065";
    }
    else if(str->equals("新北市") == true) {
        return "F-D0047-069";
    }
    else if(str->equals("臺中市") == true || str->equals("台中市") == true) {
        return "F-D0047-073";
    }
    else if(str->equals("臺南市") == true || str->equals("台南市") == true) {
        return "F-D0047-077";
    }
    else if(str->equals("連江縣") == true) {
        return "F-D0047-081";
    }
    else if(str->equals("金門縣") == true) {
        return "F-D0047-085";
    }
    return String();
}
