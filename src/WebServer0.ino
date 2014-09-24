
#include <SPI.h>
#include <Ethernet.h>


byte mac[] = { 
  0x90,0xA2,0xDA,0x0E,0x03,0xD0};
IPAddress ip(192,168,0, 200);

int analogPin1 = A0; 
float Vin = 5.0;     // [V]       Tensão de alimentação do snsor
float Rfija = 10000;  // [ohm]     Resistencia do sensor
float R25 = 10000;    // [ohm]     Valor de NTC a 25ºC
float Beta = 3988.0; // [K]      Parámetro Beta de NTC
float T0 = 293.15;   // [K]       Temperatura de referencia em Kelvin
 
float Vout = 0.0;    // [V]       Variavel saida de volts
float Rntc = 0.0;    // [ohm]     Variavel para NTC em ohm
 
float TempK = 0.0;   // [K]       Temperatura saida em Kelvin
float TempC = 0.0;   // [ºC]      Temperatura saida em Celsius


EthernetServer server(80);

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

 pinMode(analogPin1, INPUT);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  Vout=(Vin/1024)*(analogRead(analogPin1));
  Rntc=(Vout*Rfija)/(Vin-Vout);
  TempK = Beta/(log(Rntc/R25)+(Beta/T0));
  TempC = TempK-273.15;
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
     
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          // output the value of each analog input pin
        
         client.print("<h1>Schumaker Team</h1>");
            client.print("Temperatura: ");
            client.print(TempC);
            client.print(" º");
            client.print(" C");
  
            client.println("<br />");       
          
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

