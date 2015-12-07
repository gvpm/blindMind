
#include "ofApp.h"
//--------------------------------------------------------------
void ofApp::setup(){
    bug = 0;
    showInput=1;
    scale = 9;
    noiseScale = 0.02;
    cameraGray=255;
    ofEnableAlphaBlending();
    westMin=510;westMax=0;northMin=510;northMax=0;eastMin=510;eastMax=0;southMin=510;southMax=0;
    lvl = 0;
    state = 0;
    calibrationControl=0;
    duration=5000;
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
    potValue = "analog pin:";
    font.loadFont("franklinGothic.otf", 14);
    smallFont.loadFont("franklinGothic.otf", 14);
    // replace the string below with the serial port for your Arduino board
    // you can get this from the Arduino application or via command line
    // for OSX, in your terminal type "ls /dev/tty.*" to get a list of serial devices
    ard.connect("/dev/ttyACM0", 57600);
    // listen for EInitialized notification. this indicates that
    // the arduino is ready to receive commands and it is safe to
    // call setupArduino()
    ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
    bSetupArduino    = false;    // flag so we setup arduino when its ready, you don't need to touch this :)
}
//--------------------------------------------------------------
void ofApp::update(){
    middleMax = (westMax+northMax+eastMax+southMax)/4;
    middleMin= (westMin+northMin+eastMin+southMin)/4;
    lvl = ofMap(iPotValue,0,1023,10,0);
    sLvl = ofToString(lvl);

    vidGrabber.update();
    if (vidGrabber.isFrameNew())
    {
        img.setFromPixels(vidGrabber.getPixels(),imgWidth, imgHeight, OF_IMAGE_COLOR);
        img.mirror(false, true);
        imgBuffer.push_front(img);
    }
    if (imgBuffer.size()>maxBufferSize) imgBuffer.pop_back();
    /*
    int tresh = ofMap(20,(northMin+westMin)/2,(northMax+westMax)/2,0,255);
    int tresh2 = ofMap(150,(northMin+westMin)/2,(northMax+westMax)/2,0,255);
    int northMapped=ofMap(inorthLightValue,0,1023,northMin,northMax);
    int westMapped=ofMap(iwestLightValue,0,1023,westMin,westMax);
    if(abs(northMapped-westMapped)<tresh&&((northMapped+westMapped)/2)>tresh2){
        inorthWestLightValue = (northMax-northMapped)+(westMax-westMapped);
        }else{
        inorthWestLightValue = ((inorthLightValue+iwestLightValue)/2);
    }
    */
    //SIMPLE color of sensor
    westColor = ofMap(iwestLightValue,westMin,westMax,0,255);
    northColor = ofMap(inorthLightValue,northMin,northMax,0,255);
    eastColor = ofMap(ieastLightValue,eastMin,eastMax,0,255);
    southColor = ofMap(isouthLightValue,southMin,southMax,0,255);
    westX= ofGetWidth()/6;
    westY= ofGetHeight()/2;
    northX= ofGetWidth()/2;
    northY= ofGetHeight()/6;
    eastX= (ofGetWidth()/6)*5;
    eastY= ofGetHeight()/2;
    soutX= ofGetWidth()/2;
    southY= (ofGetHeight()/6)*5;


    inorthWestLightValue = ((inorthLightValue+iwestLightValue)/2);
    inorthEastLightValue = ((inorthLightValue+ieastLightValue)/2);
    isouthWestLightValue = ((isouthLightValue+iwestLightValue)/2);
    isouthEastLightValue = ((isouthLightValue+ieastLightValue)/2);
    imiddleLightValue=(iwestLightValue+inorthLightValue+ieastLightValue+isouthLightValue+inorthWestLightValue
    +inorthEastLightValue+isouthWestLightValue+isouthEastLightValue)/8;
    //UNCOMMENT THIS
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
    ard.sendAnalogPinReporting(0, ARD_ANALOG);
    ard.sendAnalogPinReporting(1, ARD_ANALOG);
    ard.sendAnalogPinReporting(2, ARD_ANALOG);
    ard.sendAnalogPinReporting(3, ARD_ANALOG);
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
    // do not send anything until the arduino has been set up
    //if (bSetupArduino) {
        // fade the led connected to pin D11
        //ard.sendPwm(11, (int)(128 + 128 * sin(ofGetElapsedTimef())));   // pwm...
    //}
}
// digital pin event handler, called whenever a digital pin value has changed
// note: if an analog pin has been set as a digital pin, it will be handled
// by the digitalPinChanged function rather than the analogPinChanged function.
//--------------------------------------------------------------
void ofApp::digitalPinChanged(const int & pinNum) {
    // do something with the digital input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    //buttonState = "digital pin: " + ofToString(pinNum) + " = " + ofToString(ard.getDigital(pinNum));
}
// analog pin event handler, called whenever an analog pin value has changed
//--------------------------------------------------------------
void ofApp::analogPinChanged(const int & pinNum) {
    // do something with the analog input. here we're simply going to print the pin number and
    // value to the screen each time it changes
    if(pinNum==0){
        //potValue = "Pot: " + ofToString(pinNum) + " = " + ofToString(ard.getAnalog(pinNum));
        westLightValue= ofToString(ard.getAnalog(pinNum));
        //  r = ofMap(ard.getAnalog(pinNum),0,1023,0,255);
        iwestLightValue = ard.getAnalog(pinNum);
    }
    if(pinNum==1){
        //lightValue = "Light: " + ofToString(pinNum) + " = " + ofToString(ard.getAnalog(pinNum));
        northLightValue = ofToString(ard.getAnalog(pinNum));
        //g = ofMap(ard.getAnalog(pinNum),0,1023,0,255);
        inorthLightValue = ard.getAnalog(pinNum);
    }
    if(pinNum==2){
        //lightValue = "Light: " + ofToString(pinNum) + " = " + ofToString(ard.getAnalog(pinNum));
        eastLightValue = ofToString(ard.getAnalog(pinNum));
        //b = ofMap(ard.getAnalog(pinNum),0,1023,0,255);
        ieastLightValue = ard.getAnalog(pinNum);
    }
    if(pinNum==3){
        //lightValue = "Light: " + ofToString(pinNum) + " = " + ofToString(ard.getAnalog(pinNum));
        southLightValue = ofToString(ard.getAnalog(pinNum));
        isouthLightValue = ard.getAnalog(pinNum);
        //g = ofMap(ard.getAnalog(pinNum),0,1023,0,255);
    }
    if(pinNum==4){
        //lightValue = "Light: " + ofToString(pinNum) + " = " + ofToString(ard.getAnalog(pinNum));
        potValue = ofToString(ard.getAnalog(pinNum));
        iPotValue = ard.getAnalog(pinNum);
        cameraGray = ofMap(ard.getAnalog(pinNum),510,1023,55,255);
        //g = ofMap(ard.getAnalog(pinNum),0,1023,0,255);
    }
}
//--------------------------------------------------------------
void ofApp::draw(){
    currentTime = ofGetElapsedTimeMillis();
    ofEnableAlphaBlending();
    ofSetColor(0, 0, 0, 127);
    ofDisableAlphaBlending();
    ofSetColor(255);
    //ARDUINO NOT WORKING
    if (!bSetupArduino){
        ofSetColor(255,0,0);
        font.drawString("arduino not ready...", 20, 35);
        //font.drawString(currentTime, 515, 40);
        //ARDUINO WORKING
        } else {
        //CALIBRATION
       if(calibrationControl==1)  {

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
        }else if(currentTime>=endTime){
            calibrationControl=0;
        }

	  }else if(calibrationControl==2){
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
	      }else if(currentTime>=endTime){
            calibrationControl=0;
        }
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
                //font.drawString(potValue, 515, 40);
                //font.drawString(lightValue, 415, 40);
                int percMapped = ofMap(iPotValue,0,1023,100,0,true);
                string percentage = ofToString(percMapped);
                string values = "Inputs\nWest Sensor: "+westLightValue+"\nNorth Sensor: "+northLightValue+"\nEast Sensor: "+eastLightValue+"\nSouth Sensor: "+southLightValue+"\nPotentiometer: "+potValue+"\nPercentage: "+percentage;
                if(showInput==1){
                font.drawString(values, 20, 35);
                }
                ofSetColor(255, 255, 255);
            }
            //PRESENTATION STATE
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

             //PRESENTATION STATE
            if(state ==1&&calibrationControl==0){
            //ofSetColor(0);
            ///EVERYTHING
            //ONE IF FOR EACH LVL
            if(lvl==0){
                //  if (imgBuffer.size()>0){
                    //ofSetColor(0,0,0,100);
                    ofSetColor(cameraGray);
                    ofImage f =  imgBuffer[0];
                    f.resize(ofGetWidth(),ofGetHeight());
                    f.draw(0,0);
                    //ofSetColor(0,0,0,50);
                    //ofRect(0,0,ofGetWidth(),ofGetHeight());
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
                    }else if(lvl==1){
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
                    }else if(lvl==2){
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
                    }else if(lvl==3){
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
                    }else if(lvl==4){
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
                    //drawBlindImageNoise(minBlind,maxBlind,reactionTresh);
/*
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




                    ofSetColor(255);
                    string values = "Values "+westLightValue+" "+northLightValue+" "+eastLightValue+" "+southLightValue+" "+potValue+" "+sLvl;
                    font.drawString(values, 215, 40);
*/
                }
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
                    }else if(lvl==8){
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
                    }else if(lvl==9){
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
                    }else if(lvl==10){
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
        if(key == OF_KEY_UP) {
            calibrationControl = 1;
            endTime = currentTime+duration;
        }
        if(key == OF_KEY_DOWN) {
            calibrationControl=2;
            endTime = currentTime+duration;
        }
        if(key == 's') {

            state = 1;
            //imgWidth = ofGetWidth();
            //imgHeight = ofGetHeight();
        }
        if(key == 'p') {
            //imgWidth = ofGetWidth()/3;
            //imgHeight = ofGetHeight()/3;
            state = 0;
        }

        if((key == 'i')) {
            //imgWidth = ofGetWidth()/3;
            //imgHeight = ofGetHeight()/3;
            showInput = 1;
        }
        if((key == 'o')) {
            //imgWidth = ofGetWidth()/3;
            //imgHeight = ofGetHeight()/3;
            showInput = 0;
        }
        if((key == 'q')) {
            //imgWidth = ofGetWidth()/3;
            //imgHeight = ofGetHeight()/3;
            bug = 0;
        }
        if((key == 'w')) {
            //imgWidth = ofGetWidth()/3;
            //imgHeight = ofGetHeight()/3;
            bug = 1;
        }

    }

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
