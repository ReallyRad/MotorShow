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
		ofxJSONElement element = new ofxJSONElement();

		if (element.parse(str) && started) {
			for (Json::ArrayIndex i = 0; i < element["DATA"].size(); i++) {				
				
				//get element at index i
				ofxJSONElement sub = element["DATA"][i];

				//display value type and timestamp
				cout << "received " << sub["ID"].asString() << "at " << sub["Timestamp"].asFloat() << " ";

				//display values separated by comma
				for (int j = 0; j<element["DATA"]["Values"].size(); j++) {
					cout << sub["Values"][j].asFloat() << ", ";
				}

				//end line
				cout << endl;
			}				
		}
			
	}
	else {

	}
}


//--------------------------------------------------------------
void ofApp::draw(){

}

void ofApp::sendStart() {
	if (client.isConnected()) {
		//send start command to server
		string str = "{ \"COMMAND\": \"start\", }";
		client.send(str);

		//receive answer
		str = client.receive();
		ofxJSONElement element = new ofxJSONElement();

		//if server answered
		if (element.parse(str)) {
			if (element["STATUS"].asString() == "OK") {				
				
				started = true;
			}
			else if (element["STATUS"].asString() == "Error") {
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
		string str = "{ COMMAND: \"stop\", }";
		client.send(str);
		str = client.receive();
		ofxJSONElement element = new ofxJSONElement();
		if (element.parse(str)) {
			if (element["STATUS"].asString() == "OK") {
				started = false;
			}
			else if (element["STATUS"].asString() == "Error") {
				ofLogError(element["MSG"].asString());
			}
		}
		else {
			ofLogError("error parsing answer from stop request");
		}

	}
}


