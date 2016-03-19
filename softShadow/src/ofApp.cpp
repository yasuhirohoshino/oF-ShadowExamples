#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    
    shadowCam.setupPerspective();
    shadowCam.setPosition(500, 1500, 500);
    shadowCam.lookAt(ofVec3f(0.0, 0.0, 0.0));
    shadowCam.setNearClip(100);
    shadowCam.setFarClip(4000.0);
    shadowCam.enableOrtho();
    shadowCam.setFov(90);
    shadowCam.setScale(2.0);
    shadowCam.setForceAspectRatio(1.0);
    
    settings.width  = 1024;
    settings.height = 1024;
    settings.textureTarget = GL_TEXTURE_2D;
    settings.internalformat = GL_R8;
    settings.useDepth = true;
    settings.depthStencilAsTexture = true;
    settings.useStencil = true;
    settings.minFilter = GL_LINEAR;
    settings.maxFilter = GL_LINEAR;
    settings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
    settings.wrapModeVertical = GL_CLAMP_TO_EDGE;
    
    settings.internalformat = GL_R8;
    depthMap.allocate(settings);
    settings.internalformat = GL_R32F;
    blurHFbo.allocate(settings);
    blurVFbo.allocate(settings);
    
    shadowShader.load("shaders/shadow");
    blurShader.load("shaders/blur");
    
    isPerspective.set("Perspective", false);
    resolution.set("Resolution", 1024, 256, 2048);
    scale.set("Scale", 2.0, 0.1, 24.0);
    sigma.set("Blur Sigma", 2.0, 1.0, 5.0);
    shadowFactor.set("Shadow Factor", 75.0, 1.0, 250.0);
    nearClip.set("Shadow Camera Near Clip", 100, 10, 1000);
    farClip.set("Shadow Camera Far Clip", 6000, 1000, 10000);
    
    gui.setup();
    gui.add(isPerspective);
    gui.add(resolution);
    gui.add(scale);
    gui.add(sigma);
    gui.add(shadowFactor);
    gui.add(nearClip);
    gui.add(farClip);
    
    isPerspective.addListener(this, &ofApp::setPerspective);
    resolution.addListener(this, &ofApp::setResolution);
    scale.addListener(this, &ofApp::setScale);
    nearClip.addListener(this, &ofApp::setNearClip);
    farClip.addListener(this, &ofApp::setFarClip);
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    time = ofGetElapsedTimef();
    shadowCam.setGlobalPosition(500 * cos(time * 2), 1500, 500 * sin(time * 2));
    shadowCam.lookAt(ofVec3f(0.0, 0.0, 0.0));
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofMatrix4x4 viewMatrix, projectionMatrix;
    viewMatrix = shadowCam.getModelViewMatrix();
    if(isPerspective){
        projectionMatrix = shadowCam.getProjectionMatrix();
    }else{
        projectionMatrix.makeOrthoMatrix(-depthMap.getWidth() * 0.5, depthMap.getWidth() * 0.5, -depthMap.getHeight() * 0.5, depthMap.getHeight() * 0.5, shadowCam.getNearClip(), shadowCam.getFarClip());
    }
    ofMatrix4x4 inverseCameraMatrix = ofMatrix4x4::getInverseOf( cam.getModelViewMatrix() );
    ofMatrix4x4 shadowTransMatrix = inverseCameraMatrix * viewMatrix * projectionMatrix * biasMatrix;
    
    depthMap.begin();
    ofClear(255,0,0, 255);
    shadowCam.begin();
    renderScene();
    shadowCam.end();
    depthMap.end();
    
    blurVFbo.begin();
    ofClear(0,255);
    blurShader.begin();
    blurShader.setUniform2f("resolution", blurHFbo.getWidth(), blurHFbo.getHeight());
    blurShader.setUniform1f("sigma", sigma);
    blurShader.setUniformTexture("blurSampler", depthMap.getDepthTexture(), 1);
    blurShader.setUniform1i("horizontal", 0);
    depthMap.getDepthTexture().draw(0, 0);
    blurShader.end();
    blurVFbo.end();
    
    blurHFbo.begin();
    ofClear(0,255);
    blurShader.begin();
    blurShader.setUniform2f("resolution", blurHFbo.getWidth(), blurHFbo.getHeight());
    blurShader.setUniform1f("sigma", sigma);
    blurShader.setUniformTexture("blurSampler", blurVFbo.getTexture(), 1);
    blurShader.setUniform1i("horizontal", 1);
    blurVFbo.draw(0, 0);
    blurShader.end();
    blurHFbo.end();
    
    cam.begin();
    shadowShader.begin();
    shadowShader.setUniformMatrix4f("lightSpaceMatrix", shadowTransMatrix);
    shadowShader.setUniformTexture("shadowMap", blurHFbo.getTexture(), 1);
    shadowShader.setUniform3f("lightPosition", shadowCam.getGlobalPosition());
    shadowShader.setUniformMatrix4f("viewMatrix", ofGetCurrentViewMatrix());
    shadowShader.setUniform1i("isPerspective", isPerspective);
    shadowShader.setUniform1f("shadowFactor", shadowFactor);
    renderScene();
    ofDrawSphere(shadowCam.getGlobalPosition(), 10);
    shadowShader.end();
    cam.end();
    
    blurHFbo.draw(0, 0, 256, 256);
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::renderScene(){
    ofEnableDepthTest();
    
    ofPushMatrix();
    ofTranslate(-350, 0, 0);
    ofRotateY(time * 50);
    ofDrawSphere(0, 250, 0, 250);
    ofPopMatrix();
    
    ofDrawBox(350, 250, 0, 500, 500, 500);
    
    ofDrawBox(0, -5, 0, 5000, 10, 5000);
    
    ofDisableDepthTest();
}

//--------------------------------------------------------------
void ofApp::setPerspective(bool &isPerspective){
    if(isPerspective){
        shadowCam.disableOrtho();
        shadowCam.setFov(90);
    }else{
        shadowCam.enableOrtho();
        shadowCam.setScale(scale);
    }
}

//--------------------------------------------------------------
void ofApp::setResolution(float &resolution){
    settings.width = resolution;
    settings.height = resolution;
    settings.internalformat = GL_R8;
    
    depthMap.allocate(settings);
    
    settings.internalformat = GL_R32F;
    blurHFbo.allocate(settings);
    blurVFbo.allocate(settings);
}

//--------------------------------------------------------------
void ofApp::setScale(float &scale){
    if(!isPerspective){
        shadowCam.setScale(scale);
    }
}

//--------------------------------------------------------------
void ofApp::setNearClip(float &nearClip){
    shadowCam.setNearClip(nearClip);
}

//--------------------------------------------------------------
void ofApp::setFarClip(float &farClip){
    shadowCam.setFarClip(farClip);
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
