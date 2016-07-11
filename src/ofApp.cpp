#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	client.setup("127.0.0.1", 5555);
	started = false;
}

//--------------------------------------------------------------
void ofApp::update(){
	if (client.isConnected()) {
		string str = client.receive();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

}

void ofApp::sendStart() {
	if (client.isConnected()) {
		string str = "{ \"COMMAND\": \"start\" }";
		client.send(str);
		str = client.receive();
		ofxJSONElement element = new ofxJSONElement();
		if (element.parse(str)) {
			if (element["Status"].asString() == "OK") {
				started = true;
			}
			else if (element["Status"].asString() == "Error") {
				ofLogError(element["MSG"].asString());
			}
		}
		else {
			ofLogError("error reading answer from start request");
		}

	}
}

void ofApp::sendStop() {
	if (client.isConnected()) {
		string str = "{ COMMAND: \"stop\" }";
		client.send(str);
		str = client.receive();
		ofxJSONElement element = new ofxJSONElement();
		if (element.parse(str)) {
			if (element["Status"].asString() == "OK") {
				started = true;
			}
			else if (element["Status"].asString() == "Error") {
				ofLogError(element["MSG"].asString());
			}
		}
		else {
			ofLogError("error parsing answer from stop request");
		}

	}
}


