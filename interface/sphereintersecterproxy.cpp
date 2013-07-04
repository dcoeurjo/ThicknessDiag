#include "sphereintersecterproxy.h"

SphereIntersecterProxy::SphereIntersecterProxy(QObject *parent):
    QObject(parent), si() {}

SphereIntersecterProxy::~SphereIntersecterProxy() {}

SphereHandle SphereIntersecterProxy::addSphere(const Sphere_3 &s)
{
    SphereHandle sh = si.add_sphere(s);
    if (sh.is_null() == false)
    { emit sphereAdded(sh); }
    return sh;
}

void SphereIntersecterProxy::removeSphere(const SphereHandle &sh)
{
    emit sphereRemoved(sh);
    si.remove_sphere(sh);
}

const SphereIntersecter& SphereIntersecterProxy::directAccess() const
{ return si; }