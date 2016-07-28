#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	client.setMessageDelimiter("\n");
	client.setup("127.0.0.1", 5000);
	receivedBytes = 0;
	if (client.isConnected()) cout << "connected to server" << endl;
	else cout << "connection failed" << endl;
	
	started = false;

	eegAlpha = new float*[4];
	eegBeta = new float*[4];
	eegTheta = new float*[4];

	for (int i = 0; i < 4; i++) {
		eegAlpha[i] = new float[8];
		eegBeta[i] = new float[8];
		eegTheta[i] = new float[8];
	}

	edaFasic = new float[8];
	edaTonic = new float[8];

	ecg = 0;

	buffer = new char[2048];
	for (int i = 0; i < 2048; i++) {
		buffer[i] = '.';
	}
	writeIndex = 0;
	//ofSetFrameRate(60);
}

//--------------------------------------------------------------
void ofApp::update(){
	if (client.isConnected() && started) {
		vector<ofxJSONElement> elements = receiveMessage();

		for (int i = 0; i < elements.size(); i++) {
			ofxJSONElement element = new ofxJSONElement();

			element = elements[i];
			//cout << "before accessing elements[DATA]" << endl;

			//if data frame
			for (Json::ArrayIndex i = 0; i < element["DATA"].size(); i++) {
				//get element at index i
				ofxJSONElement sub = elements[i]["DATA"][i];
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
			if (element["STATUS"].asString() == "OK") {
				//if request accepted
				started = true;
				cout << "start request accepted." << endl;
			}
			else if (element["STATUS"].asString() == "Error") {
				//error in the server when receiving the message
				ofLogError(element["MSG"].asString());
			}
		}
	}	
}

vector<ofxJSONElement> ofApp::receiveMessage() {
	vector<ofxJSONElement> ret;
	int startIndex = 0;
	int readIndex = 0;	
	//cout << "in read" << endl;
	//write received bytes after last written thing
	int writtenBytes = client.receiveRawBytes(buffer + (writeIndex)* sizeof(char), 700);
	if (writtenBytes > 0) {
		//look for valid JSON strings in buffer and store them in vector
		while (readIndex <= writeIndex + writtenBytes) {
			string s(buffer);
			string p(s.begin() + startIndex, s.begin() + readIndex);

			ofxJSONElement element = new ofxJSONElement();
			if (element.parse(p)) {
				ret.push_back(element);
				startIndex = readIndex;
			}
			readIndex++;
		}

		//move remainings to begining of buffer
		//for (int i = writeIndex; i < (writtenBytes + writeIndex); i++) buffer[i] = buffer[i + startIndex];

		for (int i = 0; i < (writtenBytes + writeIndex - startIndex); i++) buffer[i] = buffer[i + startIndex];
		//set index at which to write next time
		writeIndex += writtenBytes - startIndex;

		//clear the rest of the buffer just in case
		string a(buffer);
		//cout << "buffer " << a << endl;
		for (int i = writeIndex; i < 2048; i++) buffer[i] = '.';
	}
	return ret;
}

void ofApp::fitEegData(ofxJSONElement e, float ** m) {
	for (int i=0; i < 4; i++) {
		for (int j = 0; j < 8; j++) {
			m[i][j] = e["Values"][4 * i + j].asFloat();
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

	//display framerate as window title
	std::stringstream strm;
	strm << "fps: " << ofGetFrameRate();
	ofSetWindowTitle(strm.str());


	float hu = ofGetHeight() / 10;
	float wu = ofGetWidth() / 13;
	int radius = 5;

	ofDrawBitmapString("alpha", wu, 2 * hu);
	ofDrawBitmapString("beta", wu, 7 * hu);
	ofDrawBitmapString("theta", 7 * wu, 2 * hu);

	ofDrawBitmapString("EEG", 4 * wu, hu);
	ofDrawBitmapString("EDA", 10 * wu, 6 * hu);
	ofDrawBitmapString("ECG  ", 10 * wu, 9 * hu);
	ofDrawBitmapString(ecg, 12 * wu, 9*hu );

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

		//draw eeg alpha
		for (int i = 0; i < 4; i++) {
			vector<ofPoint> points;
			for (int j = 0; j < 8; j++) {
				ofPoint point = ofPoint(3 * wu + j*wu / 2, (i + 2)*hu - eegAlpha[i][j] * hu * 0.7);
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

		//draw eeg beta
		for (int i = 0; i < 4; i++) {
			vector<ofPoint> points;
			for (int j = 0; j < 8; j++) {
				ofPoint point = ofPoint(3 * wu + j*wu / 2, (i + 7)*hu - eegBeta[i][j] * hu * 0.7);
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

		//draw eeg theta
		for (int i = 0; i < 4; i++) {
			vector<ofPoint> points;
			for (int j = 0; j < 8; j++) {
				ofPoint point = ofPoint(9 * wu + j*wu / 2, (i + 2)*hu - eegAlpha[i][j] * hu *0.7);
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

		//draw eda fasic
		vector<ofPoint> fasicPoints;
		for (int j = 0; j < 8; j++) {
			ofPoint point = ofPoint(9 * wu + j*wu / 2, 7 * hu - edaFasic[j] * hu *0.7);
			fasicPoints.push_back(point);
			ofCircle(point, radius);
		}
		for (int i = 0; i < fasicPoints.size(); i++) {
			try {
				ofLine(fasicPoints.at(i - 1), fasicPoints.at(i));
			}
			catch (exception& e) { int x = 0; }
		}

		//draw eda tonic
		vector<ofPoint> tonicPoints;
		for (int j = 0; j < 8; j++) {
			ofPoint point = ofPoint(9 * wu + j*wu / 2,  8*hu - edaTonic[j] * hu *0.7);
			tonicPoints.push_back(point);
			ofCircle(point, radius);
		}
		for (int i = 0; i < tonicPoints.size(); i++) {
			try {
				ofLine(tonicPoints.at(i - 1), tonicPoints.at(i));
			}
			catch (exception& e) { int x = 0; }
		}				

	}
	ofPopMatrix();
}

void ofApp::sendCommand(string command) {
	if (client.isConnected()) {
		//send command to server
		string s = "{ \"COMMAND\": \""+ command + "\" }";
		client.send(s);										
	}
}

void ofApp::keyPressed(int key) {
	if (key == OF_KEY_UP) {
		sendCommand("start");
		started = true;
	}
	if (key == OF_KEY_DOWN) sendCommand("stop");
}

void ofApp::exit() {
	sendCommand("stop");
}