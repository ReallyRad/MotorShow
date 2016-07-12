#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//client.setMessageDelimiter("\TCP");
	client.setup("192.168.1.33", 5555);
	if (client.isConnected()) cout << "connected to server" << endl;
	else cout << "connection failed" << endl;
	started = false;
	eegAlpha = new float*[4];
	eegBeta = new float*[4];
	eegTheta = new float*[4];

	for (int i = 0; i < 8; i++) {
		eegAlpha[i] = new float[8];
		eegBeta[i] = new float[8];
		eegTheta[i] = new float[8];
	}

	edaFasic = new float[8];
	edaTonic = new float[8];
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
				cout << "received " << sub["ID"].asString() << " at " << sub["Timestamp"].asFloat() << " ";

				//fit eeg values				
				if (sub["ID"].asString() == "eeg_alpha") fitEegData(sub, eegAlpha);
				if (sub["ID"].asString() == "eeg_beta") fitEegData(sub, eegBeta);
				if (sub["ID"].asString() == "eeg_theta") fitEegData(sub, eegTheta);
				
				//display eda values				
				if (sub["ID"].asString() == "eda_tonic") fitEdaData(sub, edaFasic);
				if (sub["ID"].asString() == "eda_fasic") fitEdaData(sub, edaTonic);
				//display ecg values

				//display values separated by comma
				/*for (int j = 0; j<sub["Values"].size(); j++) {
					cout << sub["Values"][j].asFloat() << ", ";
				}
				*/
				
				//end line
				cout << endl;
			}				
		}
			
	}
	else {

	}
}

void ofApp::fitEegData(ofxJSONElement e, float ** m) {
	for (int i=0; i < 8; i++) {
		for (int j = 0; i < 4; j++) {
			m[i][j] = e["Values"][4 * j + i].asFloat();
		}		
	}	
}

void ofApp::fitEdaData(ofxJSONElement e, float * d) {
	for (int i = 0; i < 8; i++) {
		d[i] = e["Values"][i].asFloat();
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

		//wait for answer
		Sleep(1000);
		s = client.receive();
		ofxJSONElement element = new ofxJSONElement(s);

		if (element.parse(s)) {
			if (element["STATUS"].asString() == "OK") {								
				//if request accepted
				if (command == "start") started = true;
				if (command == "stop") started = false;
				cout << command << " request accepted." << endl;
			}
			else if (element["STATUS"].asString() == "Error") {
				//error in the server when receiving the message
				ofLogError(element["MSG"].asString());
			}
		}
		else {
			//error parsing the server's answer
			ofLogError("error reading answer from" + command + "request");
			if (s == "") cout << "server returned empty messsage" << endl;
		}

	}
}

void ofApp::keyPressed(int key) {
	if (key == OF_KEY_UP) sendCommand("start");
	if (key == OF_KEY_DOWN) sendCommand("stop");
}