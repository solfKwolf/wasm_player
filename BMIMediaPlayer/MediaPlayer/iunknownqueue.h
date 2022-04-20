#ifndef IUNKNOWNQUEUE_H
#define IUNKNOWNQUEUE_H

#include <vector>
#include "unknown.h"
#include "blockingqueue.h"

class IUNknownQueue
{
public:
    IUNknownQueue();
    ~IUNknownQueue();
    void init(int num, int size);
    bool push(int index, IUNknown *pIUNknown);
    void forcewake(int index);
    bool isEmpty(int index);
    IUNknown* pop(int index);
    IUNknown* front(int index);
    void resize(int size);
    int size(int index);

private:
    std::vector< BlockingQueue<IUNknown*> *> m_IUNknownVec;


};

#endif // IUNKNOWNQUEUE_H
