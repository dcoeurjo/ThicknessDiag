#ifndef VERTICAL_ORDERER_H
#define VERTICAL_ORDERER_H

#include <vector>
#include <functional>
#include <algorithm>

#ifndef NDEBUG
#  include <stdexcept>
#  include <sstream>
#endif // NDEBUG //

#include <circle_utils.h>

//struct Comp_arcs_from_point:
//    std::unary_function<bool, typename Kernel::Circular_arc_3>
//{
//    Comp_arcs_from_point(typename Kernel::Point_3 const & p):
//        point(p) {}

//    // TODO T : type to compare ?
//    //bool operator()(T const & left, T const & right) const;

//    typename Kernel::Point_3 point;
//};

template <typename Kernel, class Circle_proxy>
class Vertical_orderer
{
    public:
        Vertical_orderer(const Circle_proxy & cp):
            _circle_proxy(cp) {}

        // Concept of initial insertion:
        //  - iterators must follow STL's ForwardIterator concept
        //  - elements must be Kernel::Circular_arc_3 arcs
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
                    Comp_arcs_by_radii<Kernel>());
            typename Kernel::Point_3 start_pt(me->source()),
                     end_pt(me->target());
            // TODO initial "sort and insert"
        }

        void insert(typename Kernel::Circular_arc_3 const & arc)
        { _sorted_arcs.insert(arc); }

        void remove(typename Kernel::Circular_arc_3 const & arc)
        { _sorted_arcs.erase(arc); }

        void contains(typename Kernel::Circular_arc_3 const & arc) const
        { return _sorted_arcs.find(arc) != _sorted_arcs.end(); }

        void clear()
        { _sorted_arcs.clear(); }

    private:
        // List of arcs sorted "vertically"
        std::vector<typename Kernel::Circular_arc_3> _sorted_arcs;

        // Circle proxy
        const Circle_proxy & _circle_proxy;
};

#endif
