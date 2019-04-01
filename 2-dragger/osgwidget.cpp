#include "osgwidget.h"

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
            case GUIEventAdapter::MOVE:
                if(!view->computeIntersections(ea.getX(),ea.getY(),hits)) break;
                pointer.reset();
                pointer.setMousePosition(ea.getX(),ea.getY());
                pointer.setCamera(view->getCamera());

                for (auto hit : hits) {
                    pointer.addIntersection(hit.nodePath,hit.getLocalIntersectPoint());
                }

                for(auto itr : pointer._hitList.front().first) {
                    qDebug() << itr->getName().c_str();
                    if("flying" == itr->getName()) {
                        globelSections->addChild(itr);
                        globelScene->addChild(globelDragger);
                        globelScene->addChild(globelSections);
                        globelScene->removeChild(itr);
                    }
                }
            break;
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

                        if(newDragger)
                        {
                            newDragger->handle(pointer,ea,aa);
                            dragger = newDragger;
                            break;
                        }
                    }
                }
                return true;
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

OSGWidget::OSGWidget(QWidget* parent) : QOpenGLWidget (parent)
{
    gw = new GraphicsWindowEmbedded(0, 0, width(), height());
    world = new Group();
    globelScene = new Group();

    auto model = readNodeFile("../OpenSceneGraph-Data/glider.osg");
    globelScene->addChild(model);
    model->setName("flying");
    auto scale = model->getBound().radius() * 1.6;

    globelDragger->setupDefaultGeometry();
    globelDragger->setMatrix(Matrix::scale(scale,scale,scale) * Matrix::translate(model->getBound().center()));

    auto manager = new osgManipulator::CommandManager;
    manager->connect(*globelDragger,*globelSections);

    world->addChild(globelScene);

    viewer = new Viewer();
    viewer->setSceneData(world);
    viewer->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
    viewer->getCamera()->setGraphicsContext(gw.get());
    viewer->getCamera()->setProjectionMatrixAsPerspective(35.0, 1.0 * this->width() / this->height(), 1, 10000.0);
    viewer->getCamera()->setViewport(new Viewport(0, 0, this->width(), this->height()));
    viewer->setCameraManipulator(new osgGA::OrbitManipulator);
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

void OSGWidget::keyPressEvent(QKeyEvent* event)
{
    gw->getEventQueue()->keyPress((osgGA::GUIEventAdapter::KeySymbol) *(event->text().toUtf8().data()));
}

void OSGWidget::keyReleaseEvent(QKeyEvent* event)
{
    gw->getEventQueue()->keyRelease((osgGA::GUIEventAdapter::KeySymbol)*(event->text().toUtf8().data()));
}

