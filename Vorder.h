#ifndef VORDER_H
#define VORDER_H

#include <set>

template <typename SK>
class Vorder
{
  typedef typename SK::Circular_arc_3 Circular_arc_3;

  public:
    Vorder(/* TODO */);

    // Dictionary-like interface
    bool insert(const Circular_arc_3 &);
    bool contains(const Circular_arc_3 &) const;
    bool erase(const Circular_arc_3 &);

  private:
    std::set<Circular_arc_3> _arcs;
};

#endif // VORDER_H // vim: sw=2 et ts=2 sts=2
