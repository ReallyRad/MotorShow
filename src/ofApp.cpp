#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	client.setMessageDelimiter("\n");
	client.setup("192.168.1.33", 5555);
	started = false;
}

//--------------------------------------------------------------
void ofApp::update(){
	if (client.isConnected()) {
				
		string str = client.receive();
		
		ofxJSONElement element = new ofxJSONElement();

		if (str!= "" && element.parse(str) && started) {
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

void ofApp::sendCommand(string command) {
	if (client.isConnected()) {
		//send command to server
		string s = "{ \"COMMAND\": \""+ command + "\" }";
		client.send(s);		

		Sleep(1000);
		s = client.receive();
		ofxJSONElement element = new ofxJSONElement(s);

		if (element.parse(s)) {
			if (element["STATUS"].asString() == "OK") {								
				if (command == "start") started = true;
				if (command == "stop") started = false;
				cout << command << "request accepted." << endl;
			}
			else if (element["STATUS"].asString() == "Error") {
				ofLogError(element["MSG"].asString());
			}
		}
		else {
			ofLogError("error reading answer from" + command + "request");
			if (s == "") cout << "server returned empty messsage" << endl;
		}

	}
}

void ofApp::keyPressed(int key) {
	if (key == OF_KEY_UP) sendCommand("start");
	if (key == OF_KEY_DOWN) sendCommand("stop");
}