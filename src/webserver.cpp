#include "WiFi.h"
#include "FatFS.h"
#include "aWOT.h"

#include "webserver.hpp"

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
	file.close();
}

void initWebserver() {
	// WiFi.beginAP("Reflex");
	// app.get("/", &index);
	// server.begin();
}

void handleClient() {
	// WiFiClient client = server.accept();

	// if (client.connected()) {
	// 	app.process(&client);
	// 	client.stop();
	// }
}