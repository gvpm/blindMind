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

	void drawBlindImage(int minBlind,int maxBlind,int reactionTresh);
	void drawBlindImageNoise(int minBlind,int maxBlind,int reactionTresh);
	int getBalancedColor(int x , int y);
	int getMax (int a, int b, int c, int d);

    int showInput;
    int bug;


	ofImage				bgImage;
	ofTrueTypeFont		font;
    ofTrueTypeFont      smallFont;
	ofArduino	ard;
	bool		bSetupArduino;			// flag variable for setting up arduino once

private:

    void setupArduino(const int & version);
    void digitalPinChanged(const int & pinNum);
    void analogPinChanged(const int & pinNum);
	void updateArduino();

    //Strings to store the input values
    string potValue;
    string westLightValue;
    string northLightValue;
    string eastLightValue;
    string southLightValue;
    string sLvl;

    //Real input values
    int iPotValue;
    int iwestLightValue;
    int inorthLightValue;
    int ieastLightValue;
    int isouthLightValue;
    //Calculated colors
    int westColor,northColor,eastColor,southColor;
    //Points of the rel sensors on the screen
    int westX,westY,northX,northY,eastX,eastY,soutX,southY;
    //fake values that will be calculated
    int inorthWestLightValue;
    int inorthEastLightValue;
    int isouthWestLightValue;
    int isouthEastLightValue;
    int imiddleLightValue;

    //Video grabber variables
    ofVideoGrabber vidGrabber;
    deque<ofImage> imgBuffer;
    int imgWidth, imgHeight;
    int maxBufferSize;
    int slitSize;
    ofImage img;

    //Control variables
    int state;
    int lvl;
    int currentTime;
    int calibrationControl;
    int duration;
    int endTime;
    //Mins ans Maxs of inputs
    int westMin,westMax,northMin,northMax,eastMin,eastMax,southMin,southMax,middleMax,middleMin;
    int cameraGray;
    //To be used in the drawing functions, to make each lvl different
    int minBlind,maxBlind,reactionTresh;
    //To be used on noise
    int scale;
    float noiseScale;



};

