
#include "osgwidget.h"

osgParticle::ModularEmitter *emitter = new osgParticle::ModularEmitter;
osgParticle::ParticleSystemUpdater *psu = new osgParticle::ParticleSystemUpdater;
osgParticle::ParticleSystem *ps = new osgParticle::ParticleSystem;

Group* model;

OSGWidget::OSGWidget(QWidget* parent) : QOpenGLWidget(parent)
{
    gw = new GraphicsWindowEmbedded(0, 0, width(), height());
    world = new Group();

    model = dynamic_cast<Group*>(readNodeFile("../5-particle/原版.FBX"));
    world->addChild(model);

    //添加特效
    Group* dynamicNode = dynamic_cast<Group*>(model->getChild(0));
    std::cout << dynamicNode->getName() << " " << dynamicNode->getNumChildren() << std::endl;
    MatrixTransform* particleMatrix = dynamic_cast<MatrixTransform*>(dynamicNode->getChild(1));
    std::cout << particleMatrix->getName() << " " << particleMatrix->getNumChildren() << std::endl;
    MatrixTransform* particleMatrix2 = dynamic_cast<MatrixTransform*>(particleMatrix->getChild(0));
    std::cout << particleMatrix2->getName() << " " << particleMatrix2->getNumChildren() << std::endl;
    Geode* particleBox = dynamic_cast<Geode*>(particleMatrix2->getChild(0));
    std::cout << "radius:" << particleBox->getBound().radius() << std::endl;

    //粒子参数
    ps->setDefaultAttributes("", true, false);

    osgParticle::Particle& pexplosion = ps->getDefaultParticleTemplate();
    pexplosion.setLifeTime(3);   //粒子时间
    pexplosion.setColorRange(osgParticle::rangev4(Vec4(1,1,1,1),Vec4(0,0,1,1)));  //颜色
    pexplosion.setSizeRange(osgParticle::rangef(5.0f, 10.0f));

    osgParticle::RadialShooter *shooter = new osgParticle::RadialShooter;
    shooter->setInitialSpeedRange(100, 500);

    osgParticle::SectorPlacer *placer = new osgParticle::SectorPlacer;
    placer->setRadiusRange(osgParticle::rangef(0,particleBox->getBound().radius())); //粒子区域


    emitter->setParticleSystem(ps);
    emitter->setShooter(shooter);
    emitter->setPlacer(placer);

    osgParticle::RandomRateCounter *rrc = static_cast<osgParticle::RandomRateCounter *>(emitter->getCounter());
    rrc->setRateRange(200, 300);    // generate 20 to 30 particles per second

    psu->addParticleSystem(ps);

    particleMatrix->addChild(emitter);
    particleMatrix->addChild(psu);
    particleBox->addDrawable(ps);

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
    //添加特效
    Group* dynamicNode = dynamic_cast<Group*>(model->getChild(0));
    std::cout << dynamicNode->getName() << " " << dynamicNode->getNumChildren() << std::endl;
    MatrixTransform* particleMatrix = dynamic_cast<MatrixTransform*>(dynamicNode->getChild(1));
    std::cout << particleMatrix->getName() << " " << particleMatrix->getNumChildren() << std::endl;
    MatrixTransform* particleMatrix2 = dynamic_cast<MatrixTransform*>(particleMatrix->getChild(0));
    std::cout << particleMatrix2->getName() << " " << particleMatrix2->getNumChildren() << std::endl;
    Geode* particleBox = dynamic_cast<Geode*>(particleMatrix2->getChild(0));
    std::cout << "radius:" << particleBox->getBound().radius() << std::endl;

    particleMatrix->removeChild(emitter);
    particleMatrix->removeChild(psu);
    particleBox->removeDrawable(ps);

    gw->getEventQueue()->keyPress((osgGA::GUIEventAdapter::KeySymbol) *(event->text().toUtf8().data()));
}

void OSGWidget::keyReleaseEvent(QKeyEvent* event)
{
    gw->getEventQueue()->keyRelease((osgGA::GUIEventAdapter::KeySymbol)*(event->text().toUtf8().data()));
}

