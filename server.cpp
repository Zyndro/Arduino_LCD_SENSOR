#include <WiFi.h>
#include "server.h"
#include "display.h"
#include "sensor.h"
#include "secrets.h"
WiFiServer server(80);

String urlDecode(const String& text) {
  String decoded = "";
  char temp[] = "0x00";
  unsigned int len = text.length();
  unsigned int i = 0;
  while (i < len) {
    char c = text.charAt(i);
    if (c == '+') {
      decoded += ' ';
    } else if (c == '%' && i + 2 < len) {
      temp[2] = text.charAt(i + 1);
      temp[3] = text.charAt(i + 2);
      decoded += (char) strtol(temp, NULL, 16);
      i += 2;
    } else {
      decoded += c;
    }
    i++;
  }
  return decoded;
}

void setupWiFiAndServer() {
  showStatus("Connecting to", ssid);
  WiFi.begin(ssid, password);

  int dots = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    dots++;
    if (dots > 3) dots = 0;
    String dotsStr = "";
    for (int i = 0; i < dots; i++) {
      dotsStr += ".";
    }
    showStatus("Connecting to", ssid + dotsStr);
  }

  IPAddress ip = WiFi.localIP();
  String ipStr = ip.toString();
  Serial.println("\nWiFi connected");
  Serial.println(ipStr);
  showStatus("Connected!", ipStr);

  delay(2000); // show IP briefly before clearing
  server.begin();
}

void handleClient() {
  WiFiClient client = server.available();
  if (client) {
    float temp = getTemperature();
    float hum = getHumidity();
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("GET /?msg=") != -1) {
      int start = request.indexOf("msg=") + 4;
      int end = request.indexOf(" ", start);
      if (end == -1) end = request.length();
      String msg = urlDecode(request.substring(start, end));
      msg.trim();
      showMessage(msg);
    }

    String html = "<!DOCTYPE html><html><head><title>ESP32</title></head><body>";
    html += "<h2>Temperature and Humidity</h2>";
    html += isnan(temp) ? "<p>Temp error</p>" : "<p>Temperature: " + String(temp, 1) + " &deg;C</p>";
    html += isnan(hum) ? "<p>Hum error</p>" : "<p>Humidity: " + String(hum, 1) + " %</p>";
    html += "<form><input name='msg'><input type='submit'></form></body></html>";

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(html);
    client.stop();
  }
}
