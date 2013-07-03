#ifndef WINDOWSTATEBUILDER_H
#define WINDOWSTATEBUILDER_H

class WindowState;
class WindowStateWidget;

class WindowStateFactory
{
public:
    explicit WindowStateFactory(WindowStateWidget &wsw);

private:
    WindowState& buildConnections(WindowState &st);

    WindowStateWidget &wsw;
};

#endif // WINDOWSTATEBUILDER_H
