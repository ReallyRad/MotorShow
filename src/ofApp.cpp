#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//client.setMessageDelimiter("\TCP");
	client.setup("127.0.0.1", 5555);
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

	ecg = 0;
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
				
				//end line
				cout << endl;

				//store eeg values				
				if (sub["ID"].asString() == "eeg_alpha") fitEegData(sub, eegAlpha);
				if (sub["ID"].asString() == "eeg_beta") fitEegData(sub, eegBeta);
				if (sub["ID"].asString() == "eeg_theta") fitEegData(sub, eegTheta);

				//store eda values				
				if (sub["ID"].asString() == "eda_tonic") fitEdaData(sub, edaFasic);
				if (sub["ID"].asString() == "eda_fasic") fitEdaData(sub, edaTonic);

				//store ecg values
				if (sub["ID"].asString() == "ecg_hr") ecg = sub["Values"][0].asFloat();

			}				
		}
			
	}
	else {

	}
}

void ofApp::fitEegData(ofxJSONElement e, float ** m) {
	for (int i=0; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
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
void ofApp::draw() {

	float hu = ofGetHeight() / 10;
	float wu = ofGetWidth() / 13;
	int radius = 5;

	ofDrawBitmapString("alpha", wu, 2 * hu);
	ofDrawBitmapString("beta", wu, 7 * hu);
	ofDrawBitmapString("theta", 7 * wu, 2 * hu);

	ofDrawBitmapString("EEG", 4 * wu, hu);
	ofDrawBitmapString("EDA", 10 * wu, 6 * hu);
	ofDrawBitmapString("ECG  ", 8 * wu, 9 * hu);

	ofDrawBitmapString("ch0", 2 * wu, 2 * hu);
	ofDrawBitmapString("ch1", 2 * wu, 3 * hu);
	ofDrawBitmapString("ch2", 2 * wu, 4 * hu);
	ofDrawBitmapString("ch3", 2 * wu, 5 * hu);

	ofDrawBitmapString("ch0", 2 * wu, 7 * hu);
	ofDrawBitmapString("ch1", 2 * wu, 8 * hu);
	ofDrawBitmapString("ch2", 2 * wu, 9 * hu);
	ofDrawBitmapString("ch3", 2 * wu, 10 * hu);

	ofDrawBitmapString("ch0", 8 * wu, 2 * hu);
	ofDrawBitmapString("ch1", 8 * wu, 3 * hu);
	ofDrawBitmapString("ch2", 8 * wu, 4 * hu);
	ofDrawBitmapString("ch3", 8 * wu, 5 * hu);

	ofDrawBitmapString("fasic", 8 * wu, 7 * hu);
	ofDrawBitmapString("tonic", 8 * wu, 8 * hu);

	if (client.isConnected()) {
		ofPushMatrix();

		//draw alpha
		for (int i = 0; i < 4; i++) {
			vector<ofPoint> points;
			for (int j = 0; j < 8; j++) {
				ofPoint point = ofPoint(3 * wu + j*wu / 2, (i + 2)*hu - eegAlpha[j][i] * hu *0.7);
				points.push_back(point);
				ofCircle(point, radius);
			}
			for (int i = 0; i < points.size(); i++) {
				try {
					ofLine(points.at(i - 1), points.at(i));
				}
				catch (exception& e) { int x = 0; }
			}
		}

		//draw beta
		for (int i = 0; i < 4; i++) {
			vector<ofPoint> points;
			for (int j = 0; j < 8; j++) {
				ofPoint point = ofPoint(3 * wu + j*wu / 2, (i + 7)*hu - eegBeta[j][i] * hu *0.7);
				points.push_back(point);
				ofCircle(point, radius);
			}
			for (int i = 0; i < points.size(); i++) {
				try {
					ofLine(points.at(i - 1), points.at(i));
				}
				catch (exception& e) { int x = 0; }
			}
		}

		//draw theta
		for (int i = 0; i < 4; i++) {
			vector<ofPoint> points;
			for (int j = 0; j < 8; j++) {
				ofPoint point = ofPoint(8 * wu + j*wu / 2, (i + 2)*hu - eegAlpha[j][i] * hu *0.7);
				points.push_back(point);
				ofCircle(point, radius);
			}
			for (int i = 0; i < points.size(); i++) {
				try {
					ofLine(points.at(i - 1), points.at(i));
				}
				catch (exception& e) { int x = 0; }
			}
		}
	}
	ofPopMatrix();
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