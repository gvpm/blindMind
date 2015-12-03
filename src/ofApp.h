#pragma once

#include "ofMain.h"
#include "ofEvents.h"


class ofApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);

	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);


	ofImage				bgImage;
	ofTrueTypeFont		font;
    ofTrueTypeFont      smallFont;
	ofArduino	ard;
	bool		bSetupArduino;			// flag variable for setting up arduino once
    int r,g,b;
    int x,y;
    int angle,angleSpeed;

private:

    void setupArduino(const int & version);
    void digitalPinChanged(const int & pinNum);
    void analogPinChanged(const int & pinNum);
	void updateArduino();


    string potValue;
    string westLightValue;
    string northLightValue;
    string eastLightValue;
    string southLightValue;
    string sLvl;

    int iPotValue;


    int iwestLightValue;
    int inorthLightValue;
    int ieastLightValue;
    int isouthLightValue;

    int inorthWestLightValue;
    int inorthEastLightValue;
    int isouthWestLightValue;
    int isouthEastLightValue;

    int imiddleLightValue;

      ofVideoGrabber vidGrabber;
    deque<ofImage> imgBuffer;
    int imgWidth, imgHeight;
    int maxBufferSize;
    int slitSize;
    ofImage img;

    int state;
    int lvl;
    int currentTime;
    int calibrationControl;
    int duration;
    int endTime;
    int westMin,westMax,northMin,northMax,eastMin,eastMax,southMin,southMax;



};

