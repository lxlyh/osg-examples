#include "osgwidget.h"

OSGWidget::OSGWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    gw = new GraphicsWindowEmbedded(0, 0, width(), height());
    world = new Group();

    auto model = dynamic_cast<Group*>(readNodeFile("../5-particle/kongtiao.fbx"));

    world->addChild(model);
    viewer = new Viewer();

//    // 设置雪花类
    osg::ref_ptr<osgParticle::PrecipitationEffect> precipitationEffect = new osgParticle::PrecipitationEffect;
    // 设置雪花浓度
    precipitationEffect->snow(1);

    //设置雪花颜色
    precipitationEffect->setParticleColor(osg::Vec4(1, 1, 1, 1));

    //设置风向
    precipitationEffect->setWind(osg::Vec3(0, 1, 0.9));

    //添加特效
    auto dynamicNode = dynamic_cast<Group*>(model->getChild(0));
    std::cout << dynamicNode->getName() << " " << dynamicNode->getNumChildren() << std::endl;
    auto particleMatrix = dynamic_cast<MatrixTransform*>(dynamicNode->getChild(2));
    std::cout << particleMatrix->getName() << " " << particleMatrix->getNumChildren() << std::endl;
    auto particleBox = dynamic_cast<Geode*>(particleMatrix->getChild(0));

    auto fog = precipitationEffect->getFog();

    particleBox->addChild(precipitationEffect);

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
    gw->getEventQueue()->keyPress((osgGA::GUIEventAdapter::KeySymbol) *(event->text().toUtf8().data()));
}

void OSGWidget::keyReleaseEvent(QKeyEvent* event)
{
    gw->getEventQueue()->keyRelease((osgGA::GUIEventAdapter::KeySymbol)*(event->text().toUtf8().data()));
}

