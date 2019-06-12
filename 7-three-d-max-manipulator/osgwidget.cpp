#include "osgwidget.h"
#include <osgGA/OrbitManipulator>
#include <osgGA/NodeTrackerManipulator>

OSGWidget::OSGWidget(QWidget* parent) : QOpenGLWidget (parent)
{
    gw = new GraphicsWindowEmbedded(0, 0, width(), height());
    world = new MatrixTransform();

    auto model = readNodeFile("../OpenSceneGraph-Data/glider.osg");
    model->setName("glider");
    auto pat = new PositionAttitudeTransform();
    pat->addChild(model);
    pat->setPosition(Vec3(2,0,0));
    auto pat2 = new PositionAttitudeTransform();
    pat2->setPosition(Vec3(1,0,0));
    pat2->addChild(pat);
    world->addChild(pat2);

    auto axies = readNodeFile("../OpenSceneGraph-Data/axes.osgt");
    axies->setName("axies");
    pat = new PositionAttitudeTransform();
    pat->addChild(axies);
    pat->setPosition(Vec3(0,0,0));
    world->addChild(pat);

    viewer = new Viewer();
    viewer->setSceneData(world);

    viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
    viewer->getCamera()->setGraphicsContext(gw.get());
    viewer->getCamera()->setProjectionMatrixAsPerspective(35.0, 1.0 * this->width() / this->height(), 1, 10000.0);
    viewer->getCamera()->setViewport(new Viewport(0, 0, this->width(), this->height()));

    auto tm = new ThreeDMaxManipulator(world);
    tm->setAllowThrow(false);
    viewer->setCameraManipulator(tm);
    tm->setHomePosition(Vec3(5,-5,5),Vec3(0,0,0),Vec3(0,0,1));
    tm->setCenter(Vec3(0,0,0));

    auto handler = new PlacedEventHandler;
    viewer->addEventHandler(handler);
}

void OSGWidget::resizeGL(int width, int height)
{
    gw->getEventQueue()->windowResize(0,0,width,height);
    gw->resized(0,0,width,height);
}

void OSGWidget::paintGL()
{
    if (viewer.valid())
        viewer->frame();
    update();
}

void OSGWidget::mousePressEvent(QMouseEvent *event)
{
    int button = 0;
    switch (event->button())
    {
    case(Qt::LeftButton) :
        button = 1;
        break;
    case (Qt::MidButton) :
        button = 2;
        break;
    case (Qt::RightButton) :
        button = 3;
        break;
    case (Qt::NoButton) :
        button = 0;
        break;
    default:
        button = 0;
        break;
    }
    gw->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
}

void OSGWidget::mouseReleaseEvent(QMouseEvent *event)
{
    int button = 0;
    switch (event->button())
    {
    case(Qt::LeftButton) :
        button = 1;
        break;
    case(Qt::MidButton) :
        button = 2;
        break;
    case(Qt::RightButton) :
        button = 3;
        break;
    case(Qt::NoButton) :
        button = 0;
        break;
    default:
        button = 0;
        break;
    }
    gw->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
}

void OSGWidget::mouseMoveEvent(QMouseEvent *event)
{
    gw->getEventQueue()->mouseMotion(event->x(), event->y());
}

void OSGWidget::wheelEvent(QWheelEvent *event)
{
    gw->getEventQueue()->mouseScroll(event->delta() < 0 ? osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN);
}

void OSGWidget::keyPressEvent(QKeyEvent* event)
{
    gw->getEventQueue()->keyPress((osgGA::GUIEventAdapter::KeySymbol) event->nativeVirtualKey());
}

void OSGWidget::keyReleaseEvent(QKeyEvent* event)
{
    gw->getEventQueue()->keyRelease((osgGA::GUIEventAdapter::KeySymbol) event->nativeVirtualKey());
}

