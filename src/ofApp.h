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
	void sendStart();
	void sendStop();
		
	ofxTCPClient client;
	ofxJSONElement json;
	bool started;
};
