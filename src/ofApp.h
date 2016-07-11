#pragma once


#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxNetwork.h"

class ofApp : public ofBaseApp
{
public:
	void setup();
	void draw();

	ofxJSONElement json;

};
