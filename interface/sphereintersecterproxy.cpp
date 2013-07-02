#include "sphereintersecterproxy.h"

QSharedPointer<SphereIntersecter> SphereIntersecterProxy::instance;

SphereIntersecterProxy::SphereIntersecterProxy(QObject *parent):
    QObject(parent) {}

SphereIntersecterProxy::~SphereIntersecterProxy() {}

SphereIntersecter& SphereIntersecterProxy::internalAccess()
{
    if (instance == 0)
    { instance = QSharedPointer<SphereIntersecter>(new SphereIntersecter()); }
    return *instance;
}

const SphereIntersecter& SphereIntersecterProxy::access()
{ return internalAccess(); }

SphereHandle SphereIntersecterProxy::addSphere(const Sphere_3 &s)
{
    SphereHandle sh = internalAccess().add_sphere(s);
    emit sphereAdded(sh);
    return sh;
}

bool SphereIntersecterProxy::removeSphere(const SphereHandle &sh)
{
    if (internalAccess().remove_sphere(sh))
    { emit sphereRemoved();
        return true; }
    else
    { return false; }
}