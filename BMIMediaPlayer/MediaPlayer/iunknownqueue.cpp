#include "iunknownqueue.h"

IUNknownQueue::IUNknownQueue()
    : m_IUNknownVec(std::vector< BlockingQueue<IUNknown *>*>(0))
{

}

IUNknownQueue::~IUNknownQueue()
{

}

void IUNknownQueue::init(int num, int size)
{
    for(int i = 0;i < num; i++)
    {
        m_IUNknownVec.at(i) = new BlockingQueue<IUNknown *>(size);
    }
}

bool IUNknownQueue::push(int index, IUNknown *pIUNknown)
{
    return m_IUNknownVec.at(index)->push(pIUNknown);
}

void IUNknownQueue::forcewake(int index)
{
    m_IUNknownVec.at(index)->forcewake();
}

bool IUNknownQueue::isEmpty(int index)
{
    return m_IUNknownVec.at(index)->empty();
}

IUNknown *IUNknownQueue::pop(int index)
{
    return m_IUNknownVec.at(index)->take();
}

IUNknown *IUNknownQueue::front(int index)
{
    return m_IUNknownVec.at(index)->front();
}

void IUNknownQueue::resize(int size)
{
    m_IUNknownVec.resize(size);
}

int IUNknownQueue::size(int index)
{
    return m_IUNknownVec.at(index)->size();
}

