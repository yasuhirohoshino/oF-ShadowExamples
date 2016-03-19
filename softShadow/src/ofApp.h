#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void renderScene();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void setPerspective(bool &isPerspective);
    void setResolution(float &resolution);
    void setScale(float &scale);
    void setNearClip(float &nearClip);
    void setFarClip(float &farClip);
		
    ofEasyCam cam;
    ofCamera shadowCam;
    ofShader shadowShader, blurShader;
    ofFbo depthMap, blurVFbo, blurHFbo;
    ofFbo::Settings settings;
    float time;
    bool isOrthgraph;
    const ofMatrix4x4 biasMatrix = ofMatrix4x4(
                                               0.5, 0.0, 0.0, 0.0,
                                               0.0, 0.5, 0.0, 0.0,
                                               0.0, 0.0, 0.5, 0.0,
                                               0.5, 0.5, 0.5, 1.0
                                               );
    
    ofxPanel gui;
    ofParameter<bool> isPerspective;
    ofParameter<float> resolution;
    ofParameter<float> scale;
    ofParameter<float> sigma;
    ofParameter<float> shadowFactor;
    ofParameter<float> nearClip;
    ofParameter<float> farClip;
};