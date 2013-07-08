#ifndef WINDOWSTATEBUILDER_H
#define WINDOWSTATEBUILDER_H

#include <stdexcept>

class WindowState;
class WindowStateWidget;

class WindowStateFactory
{
public:
    explicit WindowStateFactory(WindowStateWidget &wsw);

    // Construct state given by id
    enum StateId { SPHERES, EVENT_QUEUE };
    WindowState& makeState(StateId id);

    // Exception thrown when asking for a bad state
    struct InvalidStateId: std::exception {};

private:
    WindowStateWidget &wsw;
};

#endif // WINDOWSTATEBUILDER_H
