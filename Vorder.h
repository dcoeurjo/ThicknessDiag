#ifndef VORDER_H
#define VORDER_H

#include <vector>

template <typename SK>
class Vorder
{
  typedef typename SK::Vector_3 Vector_3;
  typedef typename SK::Circular_arc_3 Circular_arc_3;

  struct Compare_arcs_at_theta
  {
    Compare_arcs_at_theta(const Vector_3 & meridian):
      meridian(meridian) {}

    bool operator()(const Circular_arc_3 & x,
        const Circular_arc_3 & y) const
    {
      // TODO
    }

    const Vector_3 & meridian;
  };

  typedef std::vector<Circular_arc_3> Arc_list;

  public:
    Vorder(const Vector_3 & meridian);

    template <typename InputIterator>
    Vorder(const Vector_3 & meridian,
        InputIterator begin,
        InputIterator end):
      _mer(meridian)
    { insert(begin, end); }


    // Insert an arc in V (keeps sorted).
    // Precondition: the arc is intersected by the current meridian
    bool insert(const Circular_arc_3 &);
    // ... template version
    template <typename InputIterator>
    void insert(InputIterator begin, InputIterator end)
    {
      for (; begin != end; begin++)
      { insert(*begin); }
    }

    bool contains(const Circular_arc_3 &) const;

    bool erase(const Circular_arc_3 &);

  private:
    Vector_3 _mer;
    Arc_list _arcs;
};

#endif // VORDER_H // vim: sw=2 et ts=2 sts=2
