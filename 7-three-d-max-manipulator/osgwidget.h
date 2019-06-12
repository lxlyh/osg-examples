#ifndef OSGWIDGET_H
#define OSGWIDGET_H

#include "placedeventhandler.h"
#include "threedmaxmanipulator.h"

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QDebug>

#include <osg/Depth>
#include <osg/PositionAttitudeTransform>
#include <osg/MatrixTransform>

using namespace osg;

#include <osgDB/ReadFile>
using namespace osgDB;

#include <osgFX/Scribe>
using namespace osgFX;

#include <osgUtil/Optimizer>
#include <osgUtil/Simplifier>
using namespace osgUtil;

#include <osgViewer/View>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
using namespace osgViewer;




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
    ref_ptr<MatrixTransform> world;
};

#endif // OSGWIDGET_H
