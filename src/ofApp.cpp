/*
 * This is a simple example use of ofArduino
 *
 * ofArduino currently only supports the standard Arduino boards
 * (UNO, Duemilanove, Diecimila, NG, and other boards based on the
 * ATMega168 or ATMega328 microcontrollers
 * The Arduio FIO and Arduino Mini should also work.
 * The Arduino MEGA and other variants based on microcontrollers
 * other than the ATMega168 and ATMega328 are not currently supported.
 *
 * To use this example, open Arduino (preferably Arduino 1.0) and
 * navigate to File -> Examples -> Firmata and open StandardFirmata.
 * Compile and upload StandardFirmata for your board, then close
 * the Arduino application and run this application.
 *
 * If you have a servo attached, press the left arrow key to rotate
 * the servo head counterclockwise and press the right arrow key to
 * rotate the servo head clockwise.
 *
 * Clicking the mouse over any part of the application will turn the
 * on-board LED on and off.
 *
 */

#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
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


	font.loadFont("franklinGothic.otf", 20);
    smallFont.loadFont("franklinGothic.otf", 14);

    // replace the string below with the serial port for your Arduino board
    // you can get this from the Arduino application or via command line
    // for OSX, in your terminal type "ls /dev/tty.*" to get a list of serial devices
	ard.connect("/dev/ttyACM0", 57600);

	// listen for EInitialized notification. this indicates that
	// the arduino is ready to receive commands and it is safe to
	// call setupArduino()
	ofAddListener(ard.EInitialized, this, &ofApp::setupArduino);
	bSetupArduino	= false;	// flag so we setup arduino when its ready, you don't need to touch this :)
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
        ofSetColor(100);
		font.drawString("arduino not ready...\n", 515, 40);
		//font.drawString(currentTime, 515, 40);

    //ARDUINO WORKING
	} else {
    //CALIBRATION
	  if(calibrationControl==1)  {

        if(currentTime < endTime){
           ofSetColor(100);
	      font.drawString("MAX CALIBRATION", 215, 40);
        string maxValues = "MaxValues "+ofToString(westMax) +" "+ofToString(northMax)+" "+ofToString(eastMax)+" "+ofToString(southMax);
		font.drawString(maxValues, 215, 140);

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
           ofSetColor(100);
	      font.drawString("MIN CALIBRATION", 215, 40);
        string minValues = "MinValues "+ofToString(westMin) +" "+ofToString(northMin)+" "+ofToString(eastMin)+" "+ofToString(southMin);
		font.drawString(minValues, 215, 140);

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
	  }else if(calibrationControl==0){
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


	    ofSetColor(100);
		//font.drawString(potValue, 515, 40);
		//font.drawString(lightValue, 415, 40);
		string values = "Values "+westLightValue+" "+northLightValue+" "+eastLightValue+" "+southLightValue+" "+potValue+" "+sLvl;
		font.drawString(values, 215, 40);
        ofSetColor(255, 255, 255);


	  }

	  //PRESENTATION STATE
	  }if(state ==1){
	      cout <<"hi";

	      //ofSetColor(0);

	  ///EVERYTHING
        //ONE IF FOR EACH LVL
        if(lvl==0){
               //  if (imgBuffer.size()>0){

                    ofImage f =  imgBuffer[0];
                    f.resize(ofGetWidth(),ofGetHeight());
                    f.draw(0,0);



        }else if(lvl==1){

            ofBackground(0,255,0);
              ofSetColor(100);

		string values = "Values "+westLightValue+" "+northLightValue+" "+eastLightValue+" "+southLightValue+" "+potValue+" "+sLvl;
		font.drawString(values, 215, 40);

        }else if(lvl==2){
            ofBackground(255,255,0);
              ofSetColor(100);

		string values = "Values "+westLightValue+" "+northLightValue+" "+eastLightValue+" "+southLightValue+" "+potValue+" "+sLvl;
		font.drawString(values, 215, 40);

        }else if(lvl==3){
            ofBackground(0,255,255);
              ofSetColor(100);

		string values = "Values "+westLightValue+" "+northLightValue+" "+eastLightValue+" "+southLightValue+" "+potValue+" "+sLvl;
		font.drawString(values, 215, 40);

        }else if(lvl==4){
            ofBackground(0,0,255);  ofSetColor(100);

		string values = "Values "+westLightValue+" "+northLightValue+" "+eastLightValue+" "+southLightValue+" "+potValue+" "+sLvl;
		font.drawString(values, 215, 40);

        }
        else if(lvl==5){
            ofBackground(0,0,255);  ofSetColor(100);

		string values = "Values "+westLightValue+" "+northLightValue+" "+eastLightValue+" "+southLightValue+" "+potValue+" "+sLvl;
		font.drawString(values, 215, 40);

        }
        else if(lvl==6){
            ofBackground(0,0,255);  ofSetColor(100);

		string values = "Values "+westLightValue+" "+northLightValue+" "+eastLightValue+" "+southLightValue+" "+potValue+" "+sLvl;
		font.drawString(values, 215, 40);

        }
        else if(lvl==7){
            ofBackground(0,0,255);  ofSetColor(100);

		string values = "Values "+westLightValue+" "+northLightValue+" "+eastLightValue+" "+southLightValue+" "+potValue+" "+sLvl;
		font.drawString(values, 215, 40);

        }else if(lvl==8){
            ofBackground(0,0,255);  ofSetColor(100);

		string values = "Values "+westLightValue+" "+northLightValue+" "+eastLightValue+" "+southLightValue+" "+potValue+" "+sLvl;
		font.drawString(values, 215, 40);

        }else if(lvl==9){

        ofBackground(0,0,255);  ofSetColor(100);









		int recWidth = ofGetWidth()/3;
        int recHeight = ofGetHeight()/3;
        int alpha = 20;
        int grayColor = 0;


    //draw rectangles on top



    for(int i =0 ; i<3;i++){
        //northwest
        if(i==0&&inorthWestLightValue>(((northMax+westMax)/2)-100)){
            grayColor = ofMap(inorthWestLightValue,((northMax+westMax)/2)-100,(northMax+westMax)/2,0,50);

        }else if(i==0){
            grayColor=0;
        }
        //north
        if(i==1&&inorthLightValue>(northMax-100)){
            grayColor = ofMap(inorthLightValue,northMax-100,northMax,0,50);

        }else if(i==1){
            grayColor=0;
        }
        //northeast
         if(i==2&&inorthEastLightValue>(((northMax+eastMax)/2)-100)){
            grayColor = ofMap(inorthEastLightValue,((northMax+eastMax)/2)-100,(northMax+eastMax)/2,0,50);

        }else if(i==2){
            grayColor=0;
        }



        ofSetColor(grayColor);
        ofRect(0+recWidth*i,0,recWidth,recHeight);

    }
    //draw rectangles on middle
     for(int i =0 ; i<3;i++){


          //west
       if(i==0&&iwestLightValue>(westMax-100)){
            grayColor = ofMap(iwestLightValue,westMax-100,westMax,0,50);

        }else if(i==0){
            grayColor=0;
        }
        //middle
        if(i==1&&imiddleLightValue>(middleMax-100)){
            grayColor = ofMap(imiddleLightValue,middleMax-100,middleMax,0,50);

        }else if(i==1){
            grayColor=0;
        }
        //east
         if(i==2&&ieastLightValue>(eastMax-100)){
            grayColor = ofMap(ieastLightValue,eastMax-100,eastMax,0,50);

        }else if(i==2){
            grayColor=0;
        }

        ofSetColor(grayColor);

        ofRect(0+recWidth*i,recHeight,recWidth,recHeight);

    }
    //draw rectangles down
    for(int i =0 ; i<3;i++){

            //southwest
        if(i==0&&isouthWestLightValue>(((southMax+westMax)/2)-100)){
            grayColor = ofMap(isouthWestLightValue,((southMax+westMax)/2)-100,(southMax+westMax)/2,0,50);

        }else if(i==0){
            grayColor=0;
        }
        //south
        if(i==1&&isouthLightValue>(southMax-100)){
            grayColor = ofMap(isouthLightValue,southMax-100,southMax,0,50);

        }else if(i==1){
            grayColor=0;
        }
        //southeast
         if(i==2&&isouthEastLightValue>(((southMax+eastMax)/2)-100)){
            grayColor = ofMap(isouthEastLightValue,((southMax+eastMax)/2)-100,(southMax+eastMax)/2,0,50);

        }else if(i==2){
            grayColor=0;
        }





        ofSetColor(grayColor);
        ofRect(0+recWidth*i,recHeight*2,recWidth,recHeight);

    }











        string values = "Values "+westLightValue+" "+northLightValue+" "+eastLightValue+" "+southLightValue+" "+potValue+" "+sLvl;
		font.drawString(values, 215, 40);

        }else if(lvl==10){
            ofBackground(0);  ofSetColor(100);

		string values = "Values "+westLightValue+" "+northLightValue+" "+eastLightValue+" "+southLightValue+" "+potValue+" "+sLvl;
		font.drawString(values, 215, 40);

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
