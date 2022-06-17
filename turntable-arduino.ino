#include <SPI.h>
#include <WiFi101.h>

#define A 0
#define B 1
#define C 2
#define D 3

class TTCommand {
  public:
  String cmd = "";
  int steps = 0;
  int delay = 1;

  TTCommand(String str) {
    int idx = str.indexOf(";");
    if (idx > 0 && idx < str.length() - 1) {
      cmd = str.substring(0, idx);
      String st = str.substring(idx + 1);
      steps = st.toInt();
    } else {
      cmd = str;
    }
  }
};

char ssid[] = "SLEGL WiFi";             //  your network SSID (name) between the " "
char pass[] = "pnr41wlan";      // your network password between the " "
int status = WL_IDLE_STATUS;      //connection status
WiFiServer server(840);            //server socket
WiFiClient client = server.available();

void write(int a,int b,int c,int d) {
  digitalWrite(A,a);
  digitalWrite(B,b);
  digitalWrite(C,c);
  digitalWrite(D,d);
}

void oneStepLeft(int d) {
  write(1,0,0,0);
  delay(d);
  write(1,1,0,0);
  delay(d);
  write(0,1,0,0);
  delay(d);
  write(0,1,1,0);
  delay(d);
  write(0,0,1,0);
  delay(d);
  write(0,0,1,1);
  delay(d);
  write(0,0,0,1);
  delay(d);
  write(1,0,0,1);
  delay(d);
}

void oneStepRight(int d) {
  write(0,0,0,1);
  delay(d);
  write(0,0,1,1);
  delay(d);
  write(0,0,1,0);
  delay(d);
  write(0,1,1,0);
  delay(d);
  write(0,1,0,0);
  delay(d);
  write(1,1,0,0);
  delay(d);
  write(1,0,0,0);
  delay(d);
  write(1,0,0,1);
  delay(d);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);

  Serial.begin(9600);
  //while (!Serial);

  enable_WiFi();
  connect_WiFi();

  server.begin();
  //printWifiStatus();
}

String line = "";

void loop() {
  if (client && client.connected()) {
    if (readStringUntil(client, line, '\n', 4096)) {
      line.trim();
      Serial.println("received: " + line);
      TTCommand cm = TTCommand(line);
      int i = 0;
      line = "";

      if (cm.cmd == "left") {
        while (i < cm.steps) {
          oneStepLeft(cm.delay);
          i++;
        }
        client.print("DONE\n");
      } else if (cm.cmd == "right") {
        while (i < cm.steps) {
          oneStepRight(cm.delay);
          i++;
        }
        client.print("DONE\n");
      } else if (cm.cmd == "PING") {
        client.print("PONG\n");
      } else {
        client.print("UNKNOWN\n");
      }
    }
  } else {
    if (client) {
      Serial.println("disconnecting client");
      client.stop();
    }
    client = server.available();
    if (client) {
      Serial.println("New Client!");
    }
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void enable_WiFi() {
  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  bool firstTime = true;
  while (status != WL_CONNECTED) {
    //Serial.print("Attempting to connect to SSID: ");
    //Serial.println(ssid);
    if (!firstTime) {
      delay(10000);
    }

    status = WiFi.begin(ssid, pass);
    firstTime = false;
  }

  digitalWrite(LED_BUILTIN, 1);
}

bool readStringUntil(WiFiClient& client, String& input, char until_c, size_t char_limit) {
    while (client.available()) {
      char c = client.read();
      input += c;
      if (c == until_c) {
        return true;
      }
      if (input.length() >= char_limit) {
        return true;
      }
    }
  return false;
}
