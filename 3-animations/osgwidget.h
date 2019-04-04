#ifndef OSGWIDGET_H
#define OSGWIDGET_H

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QDebug>

#include <osg/Depth>
#include <osg/PositionAttitudeTransform>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>
#include <osgGA/OrbitManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgFX/Scribe>
#include <osgUtil/Optimizer>
#include <osgUtil/Simplifier>
#include <osgAnimation/BasicAnimationManager>
#include <osgManipulator/Selection>
#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/CommandManager>
#include <osgViewer/View>
#include <osgFX/Outline>
#include <osg/NodeVisitor>
#include <QTime>
#include "doc.h"

using namespace osg;
using namespace osgViewer;
using namespace osgDB;
using namespace osgGA;
using namespace osgFX;
using namespace osgUtil;
using namespace osgAnimation;
using namespace std;

class OSGWidget : public QOpenGLWidget
{
    class MyCallback : public NodeCallback
    {
        BasicAnimationManager* am;
        Animation* a;
        QTime startTime;
        int frameNum,frameRate;

    public:
        MyCallback(BasicAnimationManager* am,Animation* a,int frameStart = 0,int frameNum = 60,int frameRate = 24) {
            this->am = am;
            this->a = a;
            this->frameNum = frameNum;
            this->frameRate = frameRate;
            am->playAnimation(a);
            startTime.start();

            if(frameStart > 0)
                a->setStartTime(static_cast<double>(frameStart) / frameRate);
        }

        ~MyCallback() {}

        virtual void operator()(Node* node,NodeVisitor* nv)
        {
            auto milliseconds = startTime.elapsed();
            auto seconds = milliseconds / 1000.0;
            auto num = static_cast<int>(milliseconds * frameRate / 1000.0);
            std::cout << "seconds:" << seconds << " frames:" << num << " time:" << std::endl;

            if(num < frameNum) {
                traverse(node,nv);
            }
            else {
                cout << "stop" << std::endl;
                this->am->stopAnimation(a);
                this->am->removeNestedCallback(this);
            }
        }
    } *mycallback;


public:
    OSGWidget(QWidget* parent);
    void play1();
    void play2();

protected:
    virtual void resizeGL(int width,int height);
    virtual void paintGL();

    //GUI交互
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
private:
    osgAnimation::BasicAnimationManager* anim;
    osgAnimation::AnimationList* list;

    ref_ptr<GraphicsWindowEmbedded> gw;
    ref_ptr<Viewer> viewer;
    ref_ptr<Group> world;
};

#endif // OSGWIDGET_H
