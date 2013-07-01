#ifndef SPHEREINTERSECTERPROXY_H
#define SPHEREINTERSECTERPROXY_H

#include <QSharedPointer>

#include "sphereintersecter.h"

class SphereIntersecterProxy
{
public:
    static SphereIntersecter * access();

private:
    static QSharedPointer<SphereIntersecter> instance;
};

#endif // SPHEREINTERSECTERPROXY_H
