#ifndef BLACKCONTAINER_HPP
#define BLACKCONTAINER_HPP

#include <gui_generated/containers/blackContainerBase.hpp>

class blackContainer : public blackContainerBase
{
public:
    blackContainer();
    virtual ~blackContainer() {}

    virtual void initialize();
protected:
};

#endif // BLACKCONTAINER_HPP
