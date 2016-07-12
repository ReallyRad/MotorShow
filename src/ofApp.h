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
	void eeg();
	

	ofxTCPClient client;
	ofxJSONElement json;
	bool started;	
	float** eegData;
};
