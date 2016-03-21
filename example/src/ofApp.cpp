#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

	// optionally initialize
	slicer.init(128);

	// set listener
	slicer.setListener(*this);
	
	// set end-condition
	uint8_t b[] = {13, 10};
	slicer.setEndCondition(b, sizeof(b));
	
	// set start condition
	slicer.setStartCondition("start");
	
	slicer.appendData("start");
	slicer.appendData('a');
	slicer.appendData('b');
	slicer.appendData('b');
	slicer.appendData((uint8_t)13);
	slicer.appendData((uint8_t)10);
	
	slicer.appendData("start");
	slicer.appendData("hello packet");
	slicer.appendData((uint8_t)13);
	slicer.appendData((uint8_t)10);
}

/*
 called after the endcondition was found
 */
void ofApp::onPacket(uint8_t* data, size_t datasize)
{
	char str[datasize+1];
	memset(str, 0, datasize+1);
	memcpy(str, data, datasize);
	
	ofLogNotice() << "got packet: " << string(str);
}

/*
 called only if a start-condition is set
 */
void ofApp::onPacketStart()
{
	ofLogNotice() << "packet start";
}

/*
 called when internal buffer overflows
 */
void ofApp::onPacketOverflow(uint8_t* data, size_t datasize)
{
	ofLogNotice() << "packet overflow: " << string((char*)data);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
