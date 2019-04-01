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
#include "doc.h"

using namespace osg;
using namespace osgViewer;
using namespace osgDB;
using namespace osgGA;
using namespace osgFX;
using namespace osgUtil;

class OSGWidget : public QOpenGLWidget
{
public:
    OSGWidget(QWidget* parent);

protected:
    virtual void resizeGL(int width,int height);
    virtual void paintGL();


    //GUI交互
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
private:
    ref_ptr<GraphicsWindowEmbedded> gw;
    ref_ptr<Viewer> viewer;
    ref_ptr<Group> world;
};

#endif // OSGWIDGET_H
