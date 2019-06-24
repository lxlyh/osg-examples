#include "placedeventhandler.h"

std::ostream& operator<<(std::ostream& out,Vec3 v3) {
    out << "x:" << v3.x() << " y:" << v3.y() << " z:" << v3.z();
    return out;
}

PlacedEventHandler::PlacedEventHandler() {}

PlacedEventHandler::~PlacedEventHandler() {}

Vec3 PlacedEventHandler::screen2World(osgViewer::View* view,float x,float y)
{
    Vec3 vec3;
    ref_ptr<osg::Camera> camera = view->getCamera();
    Vec3 vScreen(x, y, 0);
    Matrix m1 = camera->getViewMatrix();
    Matrix m2 = camera->getProjectionMatrix();
    Matrix m3 = camera->getViewport()->computeWindowMatrix();
    Matrix invertW,invertP,invertV;
    invertW.invert(m1); invertP.invert(m2); invertV.invert(m3);
    vec3 = vScreen * invertV;
    vec3.set(vec3.x(),vec3.y(),0);//重置深度在相机前
    vec3 = vec3 * invertP;
    vec3 = vec3 * invertW;
    vec3.set(vec3.x(),vec3.y(),vec3.z());//重置高度在平面上
    //qDebug() << vec3.x() << vec3.y() << vec3.z();
    return vec3;
}

bool PlacedEventHandler::handle(const GUIEventAdapter& ea,GUIActionAdapter& aa) {
    auto view = dynamic_cast<osgViewer::View*>(&aa);
    LineSegmentIntersector::Intersections lis;
    if(!view->computeIntersections(ea.getX(),ea.getY(),lis)) return false;

    cout << "lis num:" << lis.size() << endl;

    for(auto li : lis) {
        cout << "local:" << li.localIntersectionPoint << endl;
        cout << "world:" << li.getWorldIntersectPoint() << endl;

        auto material = dynamic_cast<Material*>(li.drawable->getOrCreateStateSet()->getAttribute(StateAttribute::MATERIAL));
        if(material != nullptr) material->setEmission(Material::FRONT,Vec4(1,0,0,1));

        cout << "material:" << material << endl;
    }

    if(GUIEventAdapter::PUSH == ea.getEventType()) {
        return true;
    } else if(GUIEventAdapter::MOVE == ea.getEventType()) {
        return true;
    } else if(GUIEventAdapter::DRAG == ea.getEventType()) return true;

    return false;
}
