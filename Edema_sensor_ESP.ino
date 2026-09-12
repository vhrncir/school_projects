#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

int counter = 100;  // Inicializace hodnoty čítače
String str;
bool asked = false;
bool messageDisplayed = false;
bool acknowledged = false;
String message = "No change"; // Default message
bool swellingDetected = false; 

SoftwareSerial espSerial(4, 5);

const char* ssid = "";     // WiFi Name (SSID)
const char* password = ""; // WiFi Password

WiFiServer server(80);  // Vytvoření instance webového serveru na portu 80

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  server.begin();
  Serial.println("Server started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  delay(100);
}

void loop() {
  if (espSerial.available() > 0) {
    message = espSerial.readStringUntil('\n');
    Serial.println("Message from Arduino: " + message);
  }

  WiFiClient client = server.available(); // Check if a client has connected
  if (client) {
    Serial.println("New client connected");
    String request = client.readStringUntil('\r');
    Serial.println("Request: " + request);
    client.flush();

    if (request.indexOf("/acknowledged") != -1) { // If "/acknowledged" URL is requested
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html><body><h1>Signal acknowledged.</h1></body></html>");
      Serial.println("Sending acknowledgment signal to Arduino...");
      espSerial.println("STOP"); // Send "STOP" message to Arduino
    } else if (request.indexOf("/get_status") != -1) { // If "/get_status" URL is requested
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println();
      client.println(message);
    } else { // Default response to client
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html><head>");
      client.println("<script>");
      client.println("setInterval(function() {");
      client.println("fetch('/get_status').then(response => response.text()).then(data => {");
      client.println("document.getElementById('status').innerText = data;");
      client.println("});");
      client.println("}, 1000);"); // Update every second
      client.println("</script>");
      client.println("</head><body>");
      client.println("<h1>Swelling Detection:</h1>");
      client.println("<h2 id='status'>Loading...</h2>");
      client.println("<p>Do you acknowledge? <a href=\"/acknowledged\">YES</a></p>");
      client.println("</body></html>");
    }
    delay(1);
    Serial.println("Client disconnected");
  }
}