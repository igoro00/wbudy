#include "aWOT.h"
#include "main.h"

WiFiServer server(80);
Application app;

void index(Request &req, Response &res) {
	File file = FatFS.open("/index.html", "r");
	if (!file.available()) {
		res.sendStatus(404);
		return;
	}
	res.set("Content-Type", "text/html");
	res.print(file.readString());
}

void initWebserver() {
	WiFi.beginAP("Kurwa");
	app.get("/", &index);
	server.begin();
}

void handleClient() {
	WiFiClient client = server.accept();

	if (client.connected()) {
		app.process(&client);
		client.stop();
	}
}