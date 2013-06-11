#ifndef EVENTS_H
#define EVENTS_H

// An event point is a point of the algorithm's base sphere
// where something relevant for the sweep process would occur.
// This notably includes:
//  1) tangency between the sweep meridian and a circle -> *critical* points
//  2) intersection between two arcs lying on the sphere (which
//      were created by the intersection of two other spheres
//      on the base sphere -> *crossing* points
//  3) tangency between two arcs lying on the sphere -> *tangency* points
//
// When cases (1) and either (2) or (3) are combined, we speak of
// *degenerate* points (crossing or tangency).

#include <utility>

// Refactoring for event structures needing a mark
// for the { Start, End } tag.
//
// Current solution is a enum.
template <typename K>
struct Tagged_event
{
  enum Tag_type {
    Start, End
  };

  Tag_type tag;

  bool is_start() const
  { return tag == Start; }

  bool is_end() const
  { return tag == End; }

  bool operator==(const Tagged_event<K> & ev) const
  { return tag == ev.tag; }
};

// Basic normal events are defined by:
//  - the pair of arcs defining its circle 
//  - a tag { Start, End }
template <typename K>
struct Normal_event: Tagged_event<K>
{
  std::pair<typename K::Circular_arc_3,
    typename K::Circular_arc_3> arcs;

  bool operator==(const Normal_event<K> & ev) const
  {
    return Tagged_event<K>::operator==(ev)
      && arcs == ev.args;
  }
};

// Polar events are defined by:
//  - the arc looping around its entire circle
//  - a tag { Start, End }
//
// Bipolar events are defined by:
//  - the arc on the circle, bounded by the poles
//  - a tag { Start, End }
//
// Both are regrouped under one single struct because
// of the very similar structure of these two. Difference
// between these is done by accessing the polarity flag.
template <typename K>
struct Polar_event: Tagged_event<K>
{
  typename K::Circular_arc_3 arc;

  enum Polarity_type {
    Single, Dual
  };

  Polarity_type polarity;

  bool is_single_polar() const
  { return polarity == Single; }

  bool is_bipolar() const
  { return polarity == Dual; }

  enum Pole_type {
    North, South
  };

  Pole_type pole;

  bool is_north() const
  { return pole == North; }

  bool is_south() const
  { return pole == North; }

  bool operator==(const Polar_event<K> & ev) const
  {
    return Tagged_event<K>::operator==(ev)
      && arc == ev.arc
      && polarity == ev.polarity
      && pole == ev.pole;
  }
};

// Intersection normal events are defined by:
//  - the intersection point (implicit ?)
//  - the pair of intersecting arcs
//  - a tag { Smallest_Crossing, Largest_Crossing, Tangency }
//
// Note: points are compared using lexicographic
//  order away from poles. Treated as a normal event.
template <typename K>
struct Intersection_event
{
  std::pair<typename K::Circular_arc_3,
    typename K::Circular_arc_3> arcs;

  enum Intersection_type {
    Smallest_Crossing,
    Largest_Crossing,
    Tangency
  };

  Intersection_type tag;

  bool operator==(const Intersection_event<K> & ev) const
  { return arcs == ev.arcs && tag == ev.tag; }
};

#endif // EVENTS_H // vim: sw=2 et ts=2 sts=2 tw=2
