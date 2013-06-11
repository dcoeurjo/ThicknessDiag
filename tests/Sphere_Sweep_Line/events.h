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

struct Normal_event
{
    // Pair of arcs defining the circle 
    // Tag { Start, End }
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

struct Intersection_event
{
    // Intersection point
    // Tag { Smallest_Crossing, Largest_Crossing, Tangency }
    //
    // Note: points are compared using lexicographic
    //  order away from poles.
};

#endif
