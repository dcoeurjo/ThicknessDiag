#include <cstdlib>
#include <iostream>

#include <CGAL/Combinatorial_map.h>
#include <CGAL/Cell_attribute.h>

struct Sum_functor
{
  template <class Cell_attribute>
  void operator()(Cell_attribute& ca1, Cell_attribute& ca2)
  {
    ca1.info() = ca1.info() + ca2.info();
  }
};

struct Divide_by_two_functor
{
  template <class Cell_attribute>
  void operator()(Cell_attribute& ca1, Cell_attribute& ca2)
  {
    ca2.info() = ca1.info() = ca1.info() / 2;
  }
};

struct Spherical_Item
{
  template <class Refs>
  struct Dart_wrapper
  {
    typedef CGAL::Cell_attribute<Refs, int, CGAL::Tag_true,
            Sum_functor, Divide_by_two_functor> Point_Attribute;

    typedef CGAL::Dart<3, Refs> Dart;
    typedef CGAL::cpp0x::tuple<Point_Attribute> Attributes;
  };
};



int main(int argc, const char * argv[])
{
  return EXIT_SUCCESS;
}

// vim: et sw=2 ts=2 sts=2
