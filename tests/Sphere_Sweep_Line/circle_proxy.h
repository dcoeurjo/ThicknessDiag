#ifndef CIRCLE_PROXY_H
#define CIRCLE_PROXY_H

#include <set>
#include <vector>
#include <utility>
#include <functional>
#include <algorithm>

template <typename Kernel>
class Circle_proxy;

template <typename Kernel>
class Circle_handle
{
    public:
        // Check if the given point lies on the circle
        bool has_on(typename Kernel::Point_3 const & p) const
        { return _circle.has_on(p); }

        // Split the circle
        //
        // Concept: points given are on the circle
        //
        // ...using one point
        typename Kernel::Circular_arc_3
            split(typename Kernel::Point_3 const & p)
        {
            typename Kernel::Circular_arc_3 arc(_circle, p);
            // TODO modify circle proxy
            return arc;
        }
        // ...using two points
        typename Kernel::Circular_arc_3
            split(typename Kernel::Point_3 const & source,
                    typename Kernel::Point_3 const & target)
        {
            typename Kernel::Circular_arc_3 arc(_circle, source, target);
            // TODO modify circle proxy
            return arc;
        }

    private:
        friend class Circle_proxy<Kernel>;

        Circle_handle(Circle_proxy<Kernel> & cp,
                typename Kernel::Circle_3 const & circle):
            _cp(cp), _circle(circle) {}

        Circle_proxy<Kernel> & _cp;
        typename Kernel::Circle_3 const & _circle;
};

template <typename Kernel>
class Circle_proxy
{
    public:

    private:
        // Set of circles
        typedef std::set<typename Kernel::Circle_3>
            Circle_list;
        Circle_list _circle_list;

        // Circle map iterator
        // ...usual
        typedef typename Circle_list::iterator
            Circle_iterator;
        // ...const
        typedef typename Circle_list::const_iterator
            Circle_const_iterator;
};

#endif
