#include "osgwidget.h"

std::ostream& operator<<(std::ostream& out,Vec3 v3) {
    out << "x:" << v3.x() << " y:" << v3.y() << " z:" << v3.z();
    return out;
}

OSGWidget::OSGWidget(QWidget* parent) : QOpenGLWidget (parent)
{
    gw = new GraphicsWindowEmbedded(0, 0, width(), height());
    world = new Group();
    globelScene = new Group();


    auto model = readNodeFile("../OpenSceneGraph-Data/cow.osg");
    globelScene->addChild(model);
    world->addChild(globelScene);

    viewer = new Viewer();
    viewer->setSceneData(world);
    viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
    viewer->getCamera()->setGraphicsContext(gw.get());

    auto ca = viewer->getCamera();
    auto om = new osgGA::OrbitManipulator;


    auto b = model->getBound();
    auto c = b.center();
    auto r = b.radius();
    auto ratio = 1. * width() / height();
    auto range = 1.0;

    auto left = (c.x() - r * ratio) * range;
    auto right = (c.x() + r * ratio) * range;
    auto bottom = (c.z() - r) * range;
    auto top = (c.z() + r) * range;
    auto near = (c.y() - r) * range;
    auto far = (c.y() + r) * range;

    ca->setProjectionMatrixAsOrtho(left,right,
                                   bottom,top,
                                   near,far);

    ca->setViewport(new Viewport(0, 0, width(),height()));
    viewer->setCameraManipulator(om,false);

    om->setCenter(Vec3(0,0,0));
    om->setRotation(Quat(DegreesToRadians(0.),X_AXIS)); //上面
    om->setRotation(Quat(DegreesToRadians(90.),X_AXIS)); //正面
    om->setRotation(Quat(DegreesToRadians(90.),Z_AXIS)); //左面
    om->setRotation(Quat(DegreesToRadians(-90.),Z_AXIS)); //右面
    om->setRotation(Quat(DegreesToRadians(180.),Z_AXIS)); //后面
    om->setRotation(Quat(DegreesToRadians(180.),X_AXIS)); //下面
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
    gw->getEventQueue()->keyPress((osgGA::GUIEventAdapter::KeySymbol) *(event->text().toUtf8().data()));
}

void OSGWidget::keyReleaseEvent(QKeyEvent* event)
{
    gw->getEventQueue()->keyRelease((osgGA::GUIEventAdapter::KeySymbol)*(event->text().toUtf8().data()));
}

