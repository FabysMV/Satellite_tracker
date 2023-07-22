#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <cmath>

class Track{
  private:
//::Datos de la red escolar BUAP ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    const char* ssid = "INFINITUM2C9E_2.4";
    const char* password = "VSdkb453dN";
    String n2yoApiKey = "LF4TJQ-5G3NWY-J3GJUV-51XR";
    const int n2yoSatelliteId = 25544;
    const int fut_sec = 1;  //Numero de posiciones para que devuelva (Son segundos y por cada segundo es una posición)
    int altitud;
    int azimuth;
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::  
     
  public:

    void Connect()
    {
       // Connect to Wi-Fi
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) { //Mientras el estado del WIFI no sea igual a conectado, imprimimos un mensaje de que está en proceso
          delay(500);
          Serial.println("Connecting to WiFi...");
        }
        Serial.println("Conectado a WiFi con IP: ");
        Serial.println(WiFi.localIP());
        delay(1000);    
    }

  void upDate()
  {
     HTTPClient client;
    if((WiFi.status() == WL_CONNECTED)){ //Si hay una conexión exitosa hacemos lo siguiente (Sobretodo se condiciona para evitar que haga peticiones al http sin sentido
      
      const int httpsPort = 443; //443 es el puerto estandar para comunicación segura en http
    
      float ref_lat = 19.003;       
      float ref_lon = -98.202;      
      float ref_alt = 2135;
    
      String url = "https://api.n2yo.com/rest/v1/satellite";
      client.begin(url + "/positions/" + String(n2yoSatelliteId) + "/" + String(ref_lat) + "/" + String(ref_lon) + "/" + String(ref_alt) + "/" + String(fut_sec) + "/&apiKey=" + n2yoApiKey);
      int httpCode = client.GET();
  
      if(httpCode > 0){ //Si la respuesta de  httpCode es mayor a 1 significa que se están recibiendo datos
        String payload = client.getString(); //payload es lo que manda de vuelta la API, es la palabra en inglés apra referirse a los datos útiles"
        Serial.println("\nStatuscode: " + String(httpCode));
        Serial.println("Datos obtenidos de N2YO \n" + payload);
  
        //En la siguiente linea se define la capacidad de manejo de datos según su tamaño y el 160 al final es un extra por si hay un excedente 
        const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 160;
        DynamicJsonDocument doc(capacity); //Esta línea lo que hace es asignar como tal la capacidad al doc, que será donde guardemos el payload para deserializarlo y poder extraer los datos que queremos
        // Parse the JSON payload
        deserializeJson(doc, payload); //Aquí deserializamos el payload en el doc de la librería JSON 
  
        /*Para extraer los datos que queremos debemos hacerlo a través de las direcciones JSON, estas se obtienen copiando el resultado de payload
          en la sig página https://jsonpathfinder.com/ con esta obtenemos los path de los datos que queremos (son los siguientes) 
          Path lalitud: x.positions[0].satlatitude
          Path longitud: x.positions[0].satlongitude
          Path altitud: x.positions[0].sataltitude
          Path azimuth: x.positions[0].azimuth
          Estas direcciones se deben transformar para poder extraer los datos con las funciones de la librería JSON quedando como en las lineas que siguen*/
        float latitud = doc["positions"][0]["satlatitude"].as<float>();     //al final se agrega un .as<float>() para convertir los datos a floar
        float longitud = doc["positions"][0]["satlongitude"].as<float>();
        float altitud = doc["positions"][0]["sataltitude"].as<float>();
        float azimuth = doc["positions"][0]["azimuth"].as<float>();
  
        Serial.print("Satellite Position - Latitude: ");
        Serial.print(latitud, 3);
        Serial.print(", Longitude: ");
        Serial.print(longitud, 3);
        Serial.print(", Altitude: ");
        Serial.print(altitud);
        Serial.println(" meters, Azimuth");
        Serial.print(azimuth);
        Serial.println("grados");
        
      }else{ //Si la respuesta es 0 entonces hay algún error :c
        Serial.println("Error on HTTP request :c");
      }
              
    }else{ //La conexión se perdió por lo que no se lleva a cabo ningun request al http 
      Serial.println("Conexion has been lost :c");
    }
    }

  int GetAz(){return azimuth;}
  int GetCen(){return altitud;}
  
  };
