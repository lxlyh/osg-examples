#include "placedeventhandler.h"
#include "threedmaxmanipulator.h"

PlacedEventHandler::PlacedEventHandler() {}

PlacedEventHandler::~PlacedEventHandler() {}

bool PlacedEventHandler::handle(const GUIEventAdapter& ea,GUIActionAdapter& aa) {
    auto view = dynamic_cast<osgViewer::View*>(&aa);

    return false;
}
