#include "osgwidget.h"

void OSGWidget::play1() {
    auto model = dynamic_cast<Switch*>(globelScene);
    auto openDoor = model->getChild(1); //获取开门动画

    am = dynamic_cast<osgAnimation::BasicAnimationManager*>(openDoor->getUpdateCallback());
    am->getAnimationList()[0]->setPlayMode(Animation::PlayMode::ONCE);
    am->playAnimation(am->getAnimationList()[0]);

    model->setValue(0,false);
    model->setValue(1,true);
}

void OSGWidget::play2() {
    auto model = dynamic_cast<Switch*>(globelScene);
    auto closeDoor = model->getChild(0);

    am = dynamic_cast<osgAnimation::BasicAnimationManager*>(closeDoor->getUpdateCallback());
    am->getAnimationList()[0]->setPlayMode(Animation::PlayMode::ONCE);
    am->playAnimation(am->getAnimationList()[0]);

    model->setValue(1,false);
    model->setValue(0,true);
}

OSGWidget::OSGWidget(QWidget* parent) : QOpenGLWidget (parent)
{
    gw = new GraphicsWindowEmbedded(0, 0, width(), height());
    world = new Group();

    auto model = new Switch();
    globelScene = model;

    //读取带动画的节点
    auto openDoor = osgDB::readNodeFile("../OpenSceneGraph-Data/开门.fbx");
    auto closeDoor = osgDB::readNodeFile("../OpenSceneGraph-Data/关门.fbx");

    model->addChild(closeDoor,true);
    model->addChild(openDoor,false);

    world->addChild(globelScene);

    viewer = new Viewer();
    viewer->setSceneData(world);
    viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
    viewer->getCamera()->setGraphicsContext(gw.get());
    viewer->getCamera()->setProjectionMatrixAsPerspective(35.0, 1.0 * this->width() / this->height(), 1, 10000.0);
    viewer->getCamera()->setViewport(new Viewport(0, 0, this->width(), this->height()));
    viewer->setCameraManipulator(new osgGA::OrbitManipulator);
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
    gw->getEventQueue()->keyPress((osgGA::GUIEventAdapter::KeySymbol) * (event->text().toUtf8().data()));
}

void OSGWidget::keyReleaseEvent(QKeyEvent* event)
{
    gw->getEventQueue()->keyRelease((osgGA::GUIEventAdapter::KeySymbol) * (event->text().toUtf8().data()));
}

