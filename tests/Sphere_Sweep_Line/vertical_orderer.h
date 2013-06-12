#ifndef VERTICAL_ORDERER_H
#define VERTICAL_ORDERER_H

#include <vector>
#include <functional>
#include <algorithm>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

//struct Comp_arcs_from_point:
//    std::unary_function<bool, typename K::Circular_arc_3>
//{
//    Comp_arcs_from_point(typename K::Point_3 const & p):
//        point(p) {}

//    // TODO T : type to compare ?
//    //bool operator()(T const & left, T const & right) const;

//    typename K::Point_3 point;
//};

template <typename K>
struct Comp_arcs_by_radii
{
    bool operator()(typename K::Circular_arc_3 const & left,
            typename K::Circular_arc_3 const & right) const
    {
        return left.squared_radius() < right.squared_radius();
    }
};

template <typename K>
class Vertical_orderer
{
    public:
        // Concept of initial insertion:
        //  - iterators must follow STL's ForwardIterator concept
        //  - elements must be K::Circular_arc_3 arcs
        //  - arcs must be tagged as either start or end,
        //      and correspond to normal events
        template <typename ForwardIterator>
        void initial_insert(ForwardIterator begin, ForwardIterator end)
        {
#ifndef NDEBUG // Check assertions
            if (_sorted_arcs.empty() == false) // Vertical orderer clean
            {
                std::ostringstream oss;
                oss << "Initial insertion can only be done when the vertical"
                    << " orderer is in a clean state, via the clear() method";
                throw std::logic_error(oss.str());
            }
            if (begin == end) // Valid input iterators
            {
                std::ostringstream oss;
                oss << "Initial insertion must be done with"
                    << " at least one element";
                throw std::logic_error(oss.str());
            }
#endif // NDEBUG //
            ForwardIterator me = max_element(begin, end, 
                    Comp_arcs_by_radii());
            typename K::Point_3 start_pt(me->source()), end_pt(me->target());
            // TODO initial "sort and insert"
        }

        void insert(typename K::Circular_arc_3 const & arc)
        { _sorted_arcs.insert(arc); }

        void remove(typename K::Circular_arc_3 const & arc)
        { _sorted_arcs.erase(arc); }

        void contains(typename K::Circular_arc_3 const & arc) const
        { return _sorted_arcs.find(arc) != _sorted_arcs.end(); }

        void clear()
        { _sorted_arcs.clear(); }

    private:
        // List of arcs sorted "vertically"
        std::vector<typename K::Circular_arc_3> _sorted_arcs;
};

#endif
