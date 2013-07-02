#ifndef SPHEREINTERSECTERPROXY_H
#define SPHEREINTERSECTERPROXY_H

#include <QObject>
#include <QSharedPointer>

#include "sphereintersecter.h"

class SphereIntersecterProxy: public QObject
{
    Q_OBJECT
public:
    explicit SphereIntersecterProxy(QObject *parent = 0);
    virtual ~SphereIntersecterProxy();

    static const SphereIntersecter& access();

    SphereHandle addSphere(const Sphere_3 &s);
    bool removeSphere(const SphereHandle &sh);

signals:
    void sphereAdded(const SphereHandle &sh);
    void sphereRemoved();

private:
    static SphereIntersecter& internalAccess();
    static QSharedPointer<SphereIntersecter> instance;
};

#endif // SPHEREINTERSECTERPROXY_H
