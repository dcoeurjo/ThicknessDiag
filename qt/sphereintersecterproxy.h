#ifndef SPHEREINTERSECTERPROXY_H
#define SPHEREINTERSECTERPROXY_H

#include <QObject>

#include "sphereintersecter.h"

class SphereIntersecterProxy: public QObject
{
    Q_OBJECT
public:
     SphereIntersecterProxy(QObject *parent = 0);
    virtual ~SphereIntersecterProxy();

    SphereHandle addSphere(const Sphere_3 &s);
    void removeSphere(const SphereHandle &sh);

    const SphereIntersecter& directAccess() const;

signals:
    void sphereAdded(const Sphere_3 &s);
    void sphereRemoved(const Sphere_3 &s);

private:
    SphereIntersecter si;
};

#endif // SPHEREINTERSECTERPROXY_H
