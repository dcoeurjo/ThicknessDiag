#ifndef VERTICAL_ORDERER_H
#define VERTICAL_ORDERER_H

#include <set>

template <typename K>
class Vertical_orderer
{
    public:
        // TODO contain/insert/delete operations
        
    private:
        // TODO write compare functor
        typedef std::less<typename K::Circular_arc_3>
            Comp_arcs_vertically;

        // BST of arcs sorted vertically
        std::set<typename K::Circular_arc_3,
            Comp_arcs_vertically> _arcs;
};

#endif
