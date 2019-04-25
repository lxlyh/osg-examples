#include "osgwidget.h"
#include <osg/PositionAttitudeTransform>

class PickModelHandler : public GUIEventHandler
{
public:
    PickModelHandler():dragger(nullptr){}

protected:
    ~PickModelHandler(){}

    osgManipulator::PointerInfo pointer;
    osgManipulator::Dragger* dragger;

    virtual bool handle(const GUIEventAdapter& ea,GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*) {
        auto view = dynamic_cast<osgViewer::View*>(&aa);
        if(!view) return false;
        osgUtil::LineSegmentIntersector::Intersections hits;

        switch(ea.getEventType()) {
            case GUIEventAdapter::PUSH:
                pointer.reset();
                if(view->computeIntersections(ea.getX(),ea.getY(),hits))
                {
                    pointer.setCamera(view->getCamera());
                    pointer.setMousePosition(ea.getX(),ea.getY());

                    for (auto hit : hits)
                        pointer.addIntersection(hit.nodePath,hit.getLocalIntersectPoint());

                    for(auto itr : pointer._hitList.front().first) {
                        auto newDragger = dynamic_cast<osgManipulator::Dragger*>(itr);

                        if(newDragger) {
                            newDragger->handle(pointer,ea,aa);
                            dragger = newDragger;
                            break;
                        }
                    }
                }
                break;
            case GUIEventAdapter::DRAG:
                if(!dragger) break;

                pointer._hitIter = pointer._hitList.begin();
                pointer.setCamera(view->getCamera());
                pointer.setMousePosition(ea.getX(),ea.getY());

                dragger->handle(pointer,ea,aa);
                return true;
            case GUIEventAdapter::RELEASE:
                dragger = nullptr;
                pointer.reset();
                break;
        }

        return false;
    }

};

auto modelPat = new PositionAttitudeTransform;
auto pat = new PositionAttitudeTransform;

OSGWidget::OSGWidget(QWidget* parent) : QOpenGLWidget (parent)
{
    gw = new GraphicsWindowEmbedded(0, 0, width(), height());
    world = new Group();
    globelScene = new Group();

    auto manager = new osgManipulator::CommandManager;
    manager->connect(*globelDragger,*globelSections);

    auto model = readNodeFile("../OpenSceneGraph-Data/animations.fbx");
    modelPat->addChild(model);
    modelPat->setPosition(Vec3d(-100,0,0));

    auto model2 = readNodeFile("../OpenSceneGraph-Data/animations.fbx");

    pat->addChild(model2);
    auto b=model2->getBound();
    pat->setPivotPoint(b.center());
    pat->setPosition(Vec3d(100,0,0));

    globelSections->addChild(modelPat);

    auto scale = model->getBound().radius() * 1.2;

    globelDragger->setMatrix(Matrix::scale(scale,scale,scale) * Matrix::translate(model->getBound().center()) * Matrix::translate(modelPat->getPosition()));
    globelDragger->setAxisLineWidth(10);
    globelDragger->setupDefaultGeometry();
    globelDragger->setHandleEvents(true);

    globelScene->addChild(pat);
    globelScene->addChild(globelDragger);
    globelScene->addChild(globelSections);

    world->addChild(globelScene);

    viewer = new Viewer();
    viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
    viewer->getCamera()->setGraphicsContext(gw.get());
    viewer->getCamera()->setProjectionMatrixAsPerspective(35.0, 1.0 * this->width() / this->height(), 1, 10000.0);
    viewer->getCamera()->setViewport(new Viewport(0, 0, this->width(), this->height()));
    viewer->setCameraManipulator(new osgGA::OrbitManipulator);
    viewer->setSceneData(world);
    viewer->addEventHandler(new PickModelHandler());
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

auto degrees = 1.0;
void OSGWidget::keyPressEvent(QKeyEvent* event)
{
    auto t = pat->getPosition();
    qDebug() << "变换前：" << t.x() << t.y() << t.z();

    auto b = pat->getChild(0)->getBound();
    qDebug() << "圆心：" << b.center().x() << b.center().y() << b.center().z();

    pat->setAttitude(pat->getAttitude() * globelSections->getMatrix().getRotate());
    qDebug() << "变换后："<< t.x() << t.y() << t.z();

//    modelPat->setPivotPoint(modelPat->getBound().center());

    gw->getEventQueue()->keyPress((osgGA::GUIEventAdapter::KeySymbol) *(event->text().toUtf8().data()));
}

void OSGWidget::keyReleaseEvent(QKeyEvent* event)
{
    gw->getEventQueue()->keyRelease((osgGA::GUIEventAdapter::KeySymbol)*(event->text().toUtf8().data()));
}

