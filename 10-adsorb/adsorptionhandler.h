#ifndef ADSORPTIONHANDLER_H
#define ADSORPTIONHANDLER_H

#include <osgGA/GUIEventHandler>
using namespace osgGA;

class AdsorptionHandler : public GUIEventHandler
{
public:
    AdsorptionHandler();
    virtual bool handle(const GUIEventAdapter& ea,GUIActionAdapter& aa);

    bool sPressed;
};

#endif // ADSORPTIONHANDLER_H
