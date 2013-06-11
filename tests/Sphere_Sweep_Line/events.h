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

// A basic normal event is defined by:
//  - the pair of arcs defining its circle 
//  - a tag { Start, End }
template <typename K>
struct Normal_event
{
  std::pair<typename K::Circular_arc_3,
    typename K::Circular_arc_3> arcs;
  bool is_start;
};

struct Polar_event
{
  // Arc looping around the entire circle
  // Tag { Start, End }
  //
  // Note: emplacement of the event is a pole
};

struct Bipolar_event
{
  // Arc on the circle, bounded by the poles
  // Tag { Start, End }
  //
  // Note: emplacement of the event is the entire meridian
};

template <typename K>
struct Intersection_event
{
  // Intersection point (implicit ?)
  // Tag { Smallest_Crossing, Largest_Crossing, Tangency }
  //
  // Note: points are compared using lexicographic
  //  order away from poles.
  //  Treated as a normal event (setup inheritance ?)
};

#endif // EVENTS_H // vim: sw=2 et ts=2 sts=2 tw=2
