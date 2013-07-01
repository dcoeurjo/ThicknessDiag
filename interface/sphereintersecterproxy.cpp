#include "sphereintersecterproxy.h"

QSharedPointer<SphereIntersecter> SphereIntersecterProxy::instance;

SphereIntersecter * SphereIntersecterProxy::access()
{
    if (instance == 0)
    { instance = QSharedPointer<SphereIntersecter>(new SphereIntersecter()); }
    return instance.data();
}
