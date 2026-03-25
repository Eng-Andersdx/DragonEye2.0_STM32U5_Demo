#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

// Forward declare Model to avoid circular includes
class Model;

class ModelListener
{
public:
    ModelListener() : model(0) {}
    virtual ~ModelListener() {}

    void bind(Model* m) { model = m; }

protected:
    Model* model;   // presenters can access this
};

#endif // MODELLISTENER_HPP
