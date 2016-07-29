#pragma once


#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxNetwork.h"

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();
	void sendCommand(string command);
	void keyPressed(int key);
	vector<ofxJSONElement> receiveMessage();
	void fitEegData(ofxJSONElement element, float ** data);
	void fitEdaData(ofxJSONElement element, float * data);
	void exit();
		
	ofxTCPClient client;
	
	ofxJSONElement json;
	
	bool started;	

	float * edaFasic;
	float * edaTonic;

	float ** eegAlpha;
	float ** eegBeta;
	float ** eegTheta;

	ofBuffer log;
	char* buffer;
	int writeIndex;

	float ecg;
};
