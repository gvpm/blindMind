
#include "ofApp.h"
//--------------------------------------------------------------
void ofApp::setup(){
    //Other version of the representation, with bugs and unfinished
    bug = 0;

    showInput=1;
    //Noise values
    scale = 9;
    noiseScale = 0.02;
    //Gray value for the first LVLS where video input is used, will be used to fade
    cameraGray=255;
    ofEnableAlphaBlending();
    //Inicial values to make the calibration work properly
    westMin=510;westMax=0;northMin=510;northMax=0;eastMin=510;eastMax=0;southMin=510;southMax=0;
    potValue = "analog pin:";

    //Keeps the current Level of blindness
    lvl = 0;
    //Keeps the current state of the program
    state = 0;
    //Keeps the calibration control status. 0=off 1=Max Calibration 2=Min Calibration
    calibrationControl=0;
    //Duration of each calibration phase
    duration=5000;

    //Setup of the videocamera device
    vidGrabber.setDeviceID(2);
    vidGrabber.setDesiredFrameRate(60);
    imgWidth = ofGetWidth()/3;
    imgHeight = ofGetHeight()/3;
    vidGrabber.initGrabber(imgWidth, imgHeight);
    slitSize = 1;
    maxBufferSize = ofGetWidth()/slitSize;
    ofSetLineWidth(5) ;
    ofSetVerticalSync(true);
    ofSetFrameRate(60);


    ofBackground(255);
    ofSetBackgroundAuto(true);

    //Sets up the printing fonts
    font.loadFont("franklinGothic.otf", 14);
    smallFont.loadFont("franklinGothic.otf", 14);
    //serial port for your Arduino board
    ard.connect("/dev/ttyACM0", 57600);
    // listen for EInitialized notification. this indicates that
    // the arduino is ready to receive commands and it is safe to
    ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino    = false;    // flag so we setup arduino when its ready, you don't need to touch this :)
}
//--------------------------------------------------------------
void ofApp::update(){

    //Represents the Min and Max of the middle tile accordint to the real values
    middleMax = (westMax+northMax+eastMax+southMax)/4;
    middleMin= (westMin+northMin+eastMin+southMin)/4;

    //Level gets the read from the potentiometer
    lvl = ofMap(iPotValue,0,1023,10,0);
    //Lvl number in a string format
    sLvl = ofToString(lvl);

    //Pushes   one image fro the camera to the buffer
    vidGrabber.update();
    if (vidGrabber.isFrameNew())
    {
        img.setFromPixels(vidGrabber.getPixels(),imgWidth, imgHeight, OF_IMAGE_COLOR);
        img.mirror(false, true);
        imgBuffer.push_front(img);
    }
    if (imgBuffer.size()>maxBufferSize) imgBuffer.pop_back();

    //SIMPLE color of sensor from REAL INPUT
    //Will calculate the gray color based on the light input taking
    //into account the Min and The max values of that environment
    westColor = ofMap(iwestLightValue,westMin,westMax,0,255);
    northColor = ofMap(inorthLightValue,northMin,northMax,0,255);
    eastColor = ofMap(ieastLightValue,eastMin,eastMax,0,255);
    southColor = ofMap(isouthLightValue,southMin,southMax,0,255);

    //Fake inputs, will calculate the values from the tiles between sensors
    inorthWestLightValue = ((inorthLightValue+iwestLightValue)/2);
    inorthEastLightValue = ((inorthLightValue+ieastLightValue)/2);
    isouthWestLightValue = ((isouthLightValue+iwestLightValue)/2);
    isouthEastLightValue = ((isouthLightValue+ieastLightValue)/2);
    imiddleLightValue=(iwestLightValue+inorthLightValue+ieastLightValue+isouthLightValue+inorthWestLightValue
    +inorthEastLightValue+isouthWestLightValue+isouthEastLightValue)/8;

    //Position of the center of each REAL SENSOR
    westX= ofGetWidth()/6;
    westY= ofGetHeight()/2;
    northX= ofGetWidth()/2;
    northY= ofGetHeight()/6;
    eastX= (ofGetWidth()/6)*5;
    eastY= ofGetHeight()/2;
    soutX= ofGetWidth()/2;
    southY= (ofGetHeight()/6)*5;


    //Update of the arduino
    updateArduino();

}
//--------------------------------------------------------------
void ofApp::setupArduino(const int & version) {
    // remove listener because we don't need it anymore
    ofRemoveListener(ard.EInitialized, this, &ofApp::setupArduino);
    // it is now safe to send commands to the Arduino
    bSetupArduino = true;
    // print firmware name and version to the console
    ofLogNotice() << ard.getFirmwareName();
    ofLogNotice() << "firmata v" << ard.getMajorFirmwareVersion() << "." << ard.getMinorFirmwareVersion();

    //SETUP OF PINS
    //WEST SENSOR
    ard.sendAnalogPinReporting(0, ARD_ANALOG);
    //NORTH SENSOR
    ard.sendAnalogPinReporting(1, ARD_ANALOG);
    //EAST SENSOR
    ard.sendAnalogPinReporting(2, ARD_ANALOG);
    //SOUTH SENSOR
    ard.sendAnalogPinReporting(3, ARD_ANALOG);
    //POTENTIOMETER
    ard.sendAnalogPinReporting(4, ARD_ANALOG);

    // Listen for changes on the digital and analog pins
    ofAddListener(ard.EDigitalPinChanged, this, &ofApp::digitalPinChanged);
    ofAddListener(ard.EAnalogPinChanged, this, &ofApp::analogPinChanged);
}
//--------------------------------------------------------------
void ofApp::updateArduino(){
    // update the arduino, get any data or messages.
    // the call to ard.update() is required
    ard.update();

}
// digital pin event handler, called whenever a digital pin value has changed
// note: if an analog pin has been set as a digital pin, it will be handled
// by the digitalPinChanged function rather than the analogPinChanged function.
//--------------------------------------------------------------
void ofApp::digitalPinChanged(const int & pinNum) {

}
// analog pin event handler, called whenever an analog pin value has changed
//--------------------------------------------------------------
void ofApp::analogPinChanged(const int & pinNum) {
    //Reads the value of the West Input
    if(pinNum==0){
        //Stores de value into a string
        westLightValue= ofToString(ard.getAnalog(pinNum));
        //Stores the int value into a variable
        iwestLightValue = ard.getAnalog(pinNum);
    }//Reads the value of the North Input
    if(pinNum==1){
        //Stores de value into a string
        northLightValue = ofToString(ard.getAnalog(pinNum));
        //Stores the int value into a variable
        inorthLightValue = ard.getAnalog(pinNum);
    }//Reads the value of the East Input
    if(pinNum==2){
        //Stores de value into a string
        eastLightValue = ofToString(ard.getAnalog(pinNum));
        //Stores the int value into a variable
        ieastLightValue = ard.getAnalog(pinNum);
    }//Reads the value of the South Input
    if(pinNum==3){
        //Stores de value into a string
        southLightValue = ofToString(ard.getAnalog(pinNum));
        //Stores the int value into a variable
        isouthLightValue = ard.getAnalog(pinNum);

    }//Reads the value of the Potentiometer Input
    if(pinNum==4){
        //Stores de value into a string
        potValue = ofToString(ard.getAnalog(pinNum));
        //Stores the int value into a variable
        iPotValue = ard.getAnalog(pinNum);
        //will fade according to the potentiometer input
        cameraGray = ofMap(ard.getAnalog(pinNum),510,1023,55,255);

    }
}
//--------------------------------------------------------------
void ofApp::draw(){

    ofEnableAlphaBlending();
    ofSetColor(0, 0, 0, 127);
    ofDisableAlphaBlending();
    ofSetColor(255);

    //Used to count the calibratin time
    currentTime = ofGetElapsedTimeMillis();

    ///ARDUINO NOT WORKING
    if (!bSetupArduino){
        ofSetColor(255,0,0);
        font.drawString("arduino not ready...", 20, 35);

        ///ARDUINO WORKING
        } else {
        ///CALIBRATION CONTROL
        //Case when Max Calibration
       if(calibrationControl==1)  {
        //Will stop when calibration time is done
        if(currentTime < endTime){
           ofSetColor(255,0,0);
	      font.drawString("MAX CALIBRATION", 20, 35);

        string maxValues = "Max West Sensor: "+ofToString(westMax) +"\nMax North Sensor:  "+ofToString(northMax)+"\nMax East Sensor:  "+ofToString(eastMax)+"\nMax South Sensor:  "+ofToString(southMax);
		font.drawString(maxValues, 20, 65);

                if(iwestLightValue>westMax)
                    westMax = iwestLightValue;
                if(inorthLightValue>northMax)
                    northMax = inorthLightValue;
                if(ieastLightValue>eastMax)
                    eastMax = ieastLightValue;
                if(isouthLightValue>southMax)
                    southMax = isouthLightValue;
        //Stops the calibration
        }else if(currentTime>=endTime){
            calibrationControl=0;
        }
    //Case when Min Calibration
	  }else if(calibrationControl==2){
	      //Will stop when calibration time is done
	      if(currentTime < endTime){
           ofSetColor(255,0,0);
	      font.drawString("MIN CALIBRATION", 20, 35);
        string minValues = "Min West Sensor: "+ofToString(westMin) +"\nMin North Sensor:  "+ofToString(northMin)+"\nMin East Sensor:  "+ofToString(eastMin)+"\nMin South Sensor:  "+ofToString(southMin);
		font.drawString(minValues, 20, 65);



                if(iwestLightValue<westMin)
                    westMin = iwestLightValue;
                if(inorthLightValue<northMin)
                    northMin = inorthLightValue;
                if(ieastLightValue<eastMin)
                    eastMin = ieastLightValue;
                if(isouthLightValue<southMin)
                    southMin = isouthLightValue;
        //Stops the calibration
	      }else if(currentTime>=endTime){
            calibrationControl=0;
        }

        ///DEFAULT STATE< INICIAL SCREEN
	  }else if(calibrationControl==0&& bug == 0){
            //DEFAULT STATE
            if (state == 0){
                int recWidth = ofGetWidth()/3;
                int recHeight = ofGetHeight()/3;
                int alpha = 20;
                int grayColor = 0;
                //draw rectangles on top
                for(int i =0 ; i<3;i++){
                    if(i==0)
                    grayColor = ofMap(inorthWestLightValue,(northMin+westMin)/2,(northMax+westMax)/2,0,255);
                    if(i==1)
                    grayColor = ofMap(inorthLightValue,northMin,northMax,0,255);
                    if(i==2)
                    grayColor = ofMap(inorthEastLightValue,(northMin+eastMin)/2,(northMax+eastMax)/2,0,255);
                    ofSetColor(grayColor);
                    ofRect(0+recWidth*i,0,recWidth,recHeight);
                }
                //draw rectangles on middle
                for(int i =0 ; i<3;i++){
                    if(i==0)
                    grayColor = ofMap(iwestLightValue,westMin,westMax,0,255);
                    if(i==1)
                    if (imgBuffer.size()>0){
                        imgBuffer[0].draw(0+recWidth*i,recHeight);
                    }
                    if(i==2)
                    grayColor = ofMap(ieastLightValue,eastMin,eastMax,0,255);
                    ofSetColor(grayColor);
                    if(i!=1)
                    ofRect(0+recWidth*i,recHeight,recWidth,recHeight);
                }

                //draw rectangles down
                for(int i =0 ; i<3;i++){
                    if(i==0)
                    grayColor = ofMap(isouthWestLightValue,(southMin+westMin)/2,(southMax+westMax)/2,0,255);
                    if(i==1)
                    grayColor = ofMap(isouthLightValue,southMin,southMax,0,255);
                    if(i==2)
                    grayColor = ofMap(isouthEastLightValue,(southMin+eastMin)/2,(southMax+eastMax)/2,0,255);
                    ofSetColor(grayColor);
                    ofRect(0+recWidth*i,recHeight*2,recWidth,recHeight);
                }
                ofSetColor(255,0,0);
                //Converts the potentiometer input in a percentage
                int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                //String to show when showinput is on
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                //shows the readed values
                if(showInput==1){
                font.drawString(values, 20, 35);
                }
                ofSetColor(255, 255, 255);
            }
//-----------------------------------------------------------------------------------------------
            ///DEFAULT STATE VERSION WITH BUG
            }else if(calibrationControl==0 && bug == 1){

                 for (int x=0; x<ofGetWidth()/scale; x++)
                    {
                    for (int y=0; y < ofGetHeight()/scale; y++)
                    {


            float noiseValue = ofNoise(x*noiseScale, y*noiseScale, float(ofGetFrameNum())*noiseScale);
            //ofSetColor(noiseValue * getBalancedColor(x,y));
            ofSetColor(getBalancedColor(x,y));
            ofRect(x*scale,y*scale,scale,scale);
        }
        }
                    ofSetColor(northColor);
                    ofCircle(northX,northY,30);

                    ofSetColor(eastColor);
                    ofCircle(eastX,eastY,30);

                    ofSetColor(southColor);
                    ofCircle(soutX,southY,30);

                    ofSetColor(westColor);
                    ofCircle(westX,westY,30);




                 ofSetColor(255,0,0);
                int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);
                }


            }
//-----------------------------------------------------------------------------------------------
             ///PRESENTATION STATE
            if(state ==1&&calibrationControl==0){


            //ONE IF FOR EACH LVL
            //The first 5 lvl will simply show the camera input and will fade it out according to the potentiometer
            ///LVL 0
            if(lvl==0){

                    ofSetColor(cameraGray);
                    ofImage f =  imgBuffer[0];
                    f.resize(ofGetWidth(),ofGetHeight());
                    f.draw(0,0);

                     ofSetColor(255,0,0);
                int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);


                }
                    ofSetColor(255,255,255);
                    percentage = ofToString(percMapped)+"%";
                    font.drawString(percentage, ofGetWidth()-60, 35);
                    }///LVL 1
                    else if(lvl==1){
                    ofSetColor(cameraGray);
                    ofImage f =  imgBuffer[0];
                    f.resize(ofGetWidth(),ofGetHeight());
                    f.draw(0,0);
                  ofSetColor(255,0,0);
                   int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);
                }ofSetColor(255,255,255);
                    percentage = ofToString(percMapped)+"%";
                    font.drawString(percentage, ofGetWidth()-60, 35);
                    }///LVL 2
                    else if(lvl==2){
                    ofSetColor(cameraGray);
                    ofImage f =  imgBuffer[0];
                    f.resize(ofGetWidth(),ofGetHeight());
                    f.draw(0,0);
                    ofSetColor(255,0,0);
                     int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);
                }ofSetColor(255,255,255);
                    percentage = ofToString(percMapped)+"%";
                    font.drawString(percentage, ofGetWidth()-60, 35);
                    }///LVL 3
                    else if(lvl==3){
                    ofSetColor(cameraGray);
                    ofImage f =  imgBuffer[0];
                    f.resize(ofGetWidth(),ofGetHeight());
                    f.draw(0,0);
               ofSetColor(255,0,0);
                int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);
                }ofSetColor(255,255,255);
                    percentage = ofToString(percMapped)+"%";
                    font.drawString(percentage, ofGetWidth()-60, 35);
                    }///LVL 4
                    else if(lvl==4){
                    ofSetColor(cameraGray);
                    ofImage f =  imgBuffer[0];
                    f.resize(ofGetWidth(),ofGetHeight());
                    f.draw(0,0);
                   ofSetColor(255,0,0);
                    int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);
                }ofSetColor(255,255,255);
                    percentage = ofToString(percMapped)+"%";
                    font.drawString(percentage, ofGetWidth()-60, 35);
                }///LVL 5
                else if(lvl==5){
                    ofSetColor(cameraGray);
                    ofImage f =  imgBuffer[0];
                    f.resize(ofGetWidth(),ofGetHeight());
                    f.draw(0,0);
                 ofSetColor(255,0,0);
                  int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);
                }ofSetColor(255,255,255);
                    percentage = ofToString(percMapped)+"%";
                    font.drawString(percentage, ofGetWidth()-60, 35);
                }
                //Starting from lvl 6 the function drawBlindImageNoise(minBlind,maxBlind,reactionTresh); is called
                //The 3 values minBlind, maxBlind and reactionTresh given to the function will be diffrent for each lvl
                ///LVL 6
                else if(lvl==6){



                    ofBackground(0,0,255);  ofSetColor(100);
                    minBlind=45;
                    maxBlind=80;
                    reactionTresh=180;

                    drawBlindImageNoise(minBlind,maxBlind,reactionTresh);
                    //drawBlindImage(minBlind,maxBlind,reactionTresh);
                    ofSetColor(255);
                ofSetColor(255,0,0);
                 int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);
                }
                ofSetColor(255,255,255);
                    percentage = ofToString(percMapped)+"%";
                    font.drawString(percentage, ofGetWidth()-60, 35);

                }///LVL 7
                else if(lvl==7){
                    ofBackground(0,0,255);  ofSetColor(100);
                    minBlind=30;
                    maxBlind=60;
                    reactionTresh=140;
                    drawBlindImageNoise(minBlind,maxBlind,reactionTresh);
                    //drawBlindImage(minBlind,maxBlind,reactionTresh);
                 ofSetColor(255,0,0);
                  int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);
                }ofSetColor(255,255,255);
                    percentage = ofToString(percMapped)+"%";
                    font.drawString(percentage, ofGetWidth()-60, 35);
                    }///LVL 8
                    else if(lvl==8){
                    ofBackground(0,0,255);  ofSetColor(100);
                    minBlind=15;
                    maxBlind=40;
                    reactionTresh=120;
                    drawBlindImageNoise(minBlind,maxBlind,reactionTresh);
                    //drawBlindImage(minBlind,maxBlind,reactionTresh);
               ofSetColor(255,0,0);
                int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);
                }ofSetColor(255,255,255);
                    percentage = ofToString(percMapped)+"%";
                    font.drawString(percentage, ofGetWidth()-60, 35);
                    }///LVL 9
                    else if(lvl==9){
                    ofBackground(0,0,255);  ofSetColor(100);
                    minBlind=0;
                    maxBlind=20;
                    reactionTresh=100;
                    drawBlindImageNoise(minBlind,maxBlind,reactionTresh);
                    //drawBlindImage(minBlind,maxBlind,reactionTresh);
                ofSetColor(255,0,0);
                 int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);
                }ofSetColor(255,255,255);
                    percentage = ofToString(percMapped)+"%";
                    font.drawString(percentage, ofGetWidth()-60, 35);
                    }///LVL 10
                    else if(lvl==10){
                    ofBackground(0);
                     ofSetColor(255,0,0);
                      int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);
                }
                ofSetColor(255,255,255);
                    percentage = ofToString(percMapped)+"%";
                    font.drawString(percentage, ofGetWidth()-60, 35);
                }
            }
        }
    }



    //--------------------------------------------------------------




    void ofApp::keyPressed  (int key){
        //Turns calibration control to Max Calibration
        if(key == OF_KEY_UP) {
            calibrationControl = 1;
            //Sets the end time of calibration
            endTime = currentTime+duration;
        }//Turns calibration control to Min Calibration
        if(key == OF_KEY_DOWN) {
            calibrationControl=2;
            //Sets the end time of calibration
            endTime = currentTime+duration;
        }//Starts the Presentation
        if(key == 's') {

            state = 1;

        }//Pauses the presentation
        if(key == 'p') {

            state = 0;
        }//Shows INPUTS on screen
        if((key == 'i')) {

            showInput = 1;
        }//Hide INPUTS from screen
        if((key == 'o')) {

            showInput = 0;
        }//Turns on the alternative mode on default screen
        if((key == 'q')) {

            bug = 0;
        }//Turns off the alternative mode on default screen
        if((key == 'w')) {

            bug = 1;
        }

    }
//Function uses in the levels
//Noise is added in that version to make it more viasuallt beautiful
//is feeded witg 3 values
//minblind=the darker the screen will get in that lvl
//
//maxvlind=the brighter the screen will get in that lvl
//
//reactiontresh= how close to the maximum value of the input the light has to be for the gray start to
//variate between the min and the max from that LVL

    void ofApp::drawBlindImageNoise(int minBlind,int maxBlind,int reactionTresh){
        int recWidth = ofGetWidth()/3;
        int recHeight = ofGetHeight()/3;
        int alpha = 20;
        int grayColor = minBlind;

     //draw rectangles on top
    for(int i =0 ; i<3;i++){
            //northwest
            if(i==0&&inorthWestLightValue>(((northMax+westMax)/2)-reactionTresh)){
                grayColor = ofMap(inorthWestLightValue,((northMax+westMax)/2)-reactionTresh,(northMax+westMax)/2,minBlind,maxBlind);
                }else if(i==0){
                grayColor=minBlind;
            }
            //north
            if(i==1&&inorthLightValue>(northMax-reactionTresh)){
                grayColor = ofMap(inorthLightValue,northMax-reactionTresh,northMax,minBlind,maxBlind);
                }else if(i==1){
                grayColor=minBlind;
            }
            //northeast
            if(i==2&&inorthEastLightValue>(((northMax+eastMax)/2)-reactionTresh)){
                grayColor = ofMap(inorthEastLightValue,((northMax+eastMax)/2)-reactionTresh,(northMax+eastMax)/2,minBlind,maxBlind);
                }else if(i==2){
                grayColor=minBlind;
            }


        for (int x=0+(recWidth/scale)*i; x<(0+recWidth*i)+recWidth/scale; x++){
        for (int y=0; y < recHeight/scale; y++){



            float noiseValue = ofNoise(x*noiseScale, y*noiseScale, float(ofGetFrameNum())*noiseScale);
            ofSetColor(noiseValue * grayColor);
            ofRect(x*scale,y*scale,scale,scale);
        }
        }

        }


         //draw rectangles on middle
        for(int i =0 ; i<3;i++){
            //west
            if(i==0&&iwestLightValue>(westMax-reactionTresh)){
                grayColor = ofMap(iwestLightValue,westMax-reactionTresh,westMax,minBlind,maxBlind);
                }else if(i==0){
                grayColor=minBlind;
            }
            //middle
            if(i==1&&imiddleLightValue>(middleMax-reactionTresh)){
                grayColor = ofMap(imiddleLightValue,middleMax-reactionTresh,middleMax,minBlind,maxBlind);
                }else if(i==1){
                grayColor=minBlind;
            }
            //east
            if(i==2&&ieastLightValue>(eastMax-reactionTresh)){
                grayColor = ofMap(ieastLightValue,eastMax-reactionTresh,eastMax,minBlind,maxBlind);
                }else if(i==2){
                grayColor=minBlind;
            }
             for (int x=0+(recWidth/scale)*i; x<(0+recWidth*i)+recWidth/scale; x++){
        for (int y=recHeight/scale; y < (recHeight/scale)+recHeight/scale; y++){



            float noiseValue = ofNoise(x*noiseScale, y*noiseScale, float(ofGetFrameNum())*noiseScale);
            ofSetColor(noiseValue * grayColor);
            ofRect(x*scale,y*scale,scale,scale);
        }
        }
        }
        //draw rectangles down
        for(int i =0 ; i<3;i++){
            //southwest
            if(i==0&&isouthWestLightValue>(((southMax+westMax)/2)-reactionTresh)){
                grayColor = ofMap(isouthWestLightValue,((southMax+westMax)/2)-reactionTresh,(southMax+westMax)/2,minBlind,maxBlind);
                }else if(i==0){
                grayColor=minBlind;
            }
            //south
            if(i==1&&isouthLightValue>(southMax-reactionTresh)){
                grayColor = ofMap(isouthLightValue,southMax-reactionTresh,southMax,minBlind,maxBlind);
                }else if(i==1){
                grayColor=minBlind;
            }
            //southeast
            if(i==2&&isouthEastLightValue>(((southMax+eastMax)/2)-reactionTresh)){
                grayColor = ofMap(isouthEastLightValue,((southMax+eastMax)/2)-reactionTresh,(southMax+eastMax)/2,minBlind,maxBlind);
                }else if(i==2){
                grayColor=minBlind;
            }
             for (int x=0+(recWidth/scale)*i; x<(0+recWidth*i)+recWidth/scale; x++){
        for (int y=(recHeight/scale)*2; y < (recHeight/scale)*3; y++){



            float noiseValue = ofNoise(x*noiseScale, y*noiseScale, float(ofGetFrameNum())*noiseScale);
            ofSetColor(noiseValue * grayColor);
            ofRect(x*scale,y*scale,scale,scale);
        }
        }
        }





    }


//Up function with no noise
//is feeded witg 3 values
//minblind=the darker the screen will get in that lvl
//
//maxvlind=the brighter the screen will get in that lvl
//
//reactiontresh= how close to the maximum value of the input the light has to be for the gray start to
//variate between the min and the max from that LVL

    void ofApp::drawBlindImage(int minBlind,int maxBlind,int reactionTresh){
        int recWidth = ofGetWidth()/3;
        int recHeight = ofGetHeight()/3;
        int alpha = 20;
        int grayColor = minBlind;
        //draw rectangles on top
        for(int i =0 ; i<3;i++){
            //northwest
            if(i==0&&inorthWestLightValue>(((northMax+westMax)/2)-reactionTresh)){
                grayColor = ofMap(inorthWestLightValue,((northMax+westMax)/2)-reactionTresh,(northMax+westMax)/2,minBlind,maxBlind);
                }else if(i==0){
                grayColor=minBlind;
            }
            //north
            if(i==1&&inorthLightValue>(northMax-reactionTresh)){
                grayColor = ofMap(inorthLightValue,northMax-reactionTresh,northMax,minBlind,maxBlind);
                }else if(i==1){
                grayColor=minBlind;
            }
            //northeast
            if(i==2&&inorthEastLightValue>(((northMax+eastMax)/2)-reactionTresh)){
                grayColor = ofMap(inorthEastLightValue,((northMax+eastMax)/2)-reactionTresh,(northMax+eastMax)/2,minBlind,maxBlind);
                }else if(i==2){
                grayColor=minBlind;
            }




            ofSetColor(grayColor);
            ofRect(0+recWidth*i,0,recWidth,recHeight);
        }
        //draw rectangles on middle
        for(int i =0 ; i<3;i++){
            //west
            if(i==0&&iwestLightValue>(westMax-reactionTresh)){
                grayColor = ofMap(iwestLightValue,westMax-reactionTresh,westMax,minBlind,maxBlind);
                }else if(i==0){
                grayColor=minBlind;
            }
            //middle
            if(i==1&&imiddleLightValue>(middleMax-reactionTresh)){
                grayColor = ofMap(imiddleLightValue,middleMax-reactionTresh,middleMax,minBlind,maxBlind);
                }else if(i==1){
                grayColor=minBlind;
            }
            //east
            if(i==2&&ieastLightValue>(eastMax-reactionTresh)){
                grayColor = ofMap(ieastLightValue,eastMax-reactionTresh,eastMax,minBlind,maxBlind);
                }else if(i==2){
                grayColor=minBlind;
            }
            ofSetColor(grayColor);
            ofRect(0+recWidth*i,recHeight,recWidth,recHeight);
        }
        //draw rectangles down
        for(int i =0 ; i<3;i++){
            //southwest
            if(i==0&&isouthWestLightValue>(((southMax+westMax)/2)-reactionTresh)){
                grayColor = ofMap(isouthWestLightValue,((southMax+westMax)/2)-reactionTresh,(southMax+westMax)/2,minBlind,maxBlind);
                }else if(i==0){
                grayColor=minBlind;
            }
            //south
            if(i==1&&isouthLightValue>(southMax-reactionTresh)){
                grayColor = ofMap(isouthLightValue,southMax-reactionTresh,southMax,minBlind,maxBlind);
                }else if(i==1){
                grayColor=minBlind;
            }
            //southeast
            if(i==2&&isouthEastLightValue>(((southMax+eastMax)/2)-reactionTresh)){
                grayColor = ofMap(isouthEastLightValue,((southMax+eastMax)/2)-reactionTresh,(southMax+eastMax)/2,minBlind,maxBlind);
                }else if(i==2){
                grayColor=minBlind;
            }
            ofSetColor(grayColor);
            ofRect(0+recWidth*i,recHeight*2,recWidth,recHeight);
        }
    }

//returns the maximum value of the inputs
    int ofApp::getMax (int a, int b, int c, int d){
        if(a*3>b+c+d){
            return a;
        }if(b*3>a+c+d){
            return b;
        }if(c*3>b+a+d){
            return c;
        }if(d*3>b+c+a){
            return d;
        }
    }
//will get the color balanced according to the distances to the real sensors
    int ofApp::getBalancedColor (int x, int y){
         int dn,de,dw,ds,dsum;
         int balanced;
         int higherDist;

         dn = fabs(ofDist(x,y,northX,northY));

         //cout<<ofToString(dn)+" ";
         de = fabs(ofDist(x,y,eastX,eastY));
         //cout<<ofToString(de)+" ";
         dw = fabs(ofDist(x,y,westX,westY));
         //cout<<ofToString(dw)+" ";
         ds = fabs(ofDist(x,y,soutX,southY));
         //cout<<"          "+ofToString(dn)+" "+ofToString(dw)+" "+ofToString(de)+" "+ofToString(ds)+"           ";

         //cout<<ofToString(ds)+"  ";
         higherDist = getMax(dn,de,dw,ds);

         dn = fabs(dn-higherDist);
         de = fabs(de-higherDist);
         dw = fabs(dw-higherDist);
         ds = fabs(ds-higherDist);


         dsum = dn+de+dw+ds;

        /*
         if(dn*3>de+dw+ds){
            balanced = ((northColor*dn)+(eastColor*de)+(westColor*dw))/(dn+de+dw);
         }else if(de*3>dn+dw+ds){
            balanced = ((northColor*dn)+(eastColor*de)+(southColor*ds))/(dn+ds+de);
         }else if(dw*3>dn+de+ds){
            balanced = ((northColor*dn)+(southColor*ds)+(westColor*dw))/(de+ds+dw);
         }else if(ds*3>dn+de+dw){
            balanced = ((southColor*ds)+(eastColor*de)+(westColor*dw))/(ds+de+dw);
         }
        */

         balanced = ((northColor*dn)+(eastColor*de)+(westColor*dw)+(southColor*ds))/dsum;
         balanced = ofMap(balanced, 0, mouseY,0,255,true);



        return balanced;

    }
    //--------------------------------------------------------------
    void ofApp::keyReleased(int key){
    }
    //--------------------------------------------------------------
    void ofApp::mouseMoved(int x, int y ){
    }
    //--------------------------------------------------------------
    void ofApp::mouseDragged(int x, int y, int button){
    }
    //--------------------------------------------------------------
    void ofApp::mousePressed(int x, int y, int button){
        // turn on the onboard LED when the application window is clicked
        ard.sendDigital(13, ARD_HIGH);
    }
    //--------------------------------------------------------------
    void ofApp::mouseReleased(int x, int y, int button){
        // turn off the onboard LED when the application window is clicked
        ard.sendDigital(13, ARD_LOW);
    }
    //--------------------------------------------------------------
    void ofApp::windowResized(int w, int h){
    }
    //--------------------------------------------------------------
    void ofApp::gotMessage(ofMessage msg){
    }
    //--------------------------------------------------------------
    void ofApp::dragEvent(ofDragInfo dragInfo){
    }
