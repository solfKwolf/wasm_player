#ifndef BLOCKINGQUEUE
#define BLOCKINGQUEUE
#include <mutex>
#include <condition_variable>
#include <deque>
#include <utility>
#include <boost/noncopyable.hpp>
#include <functional>

template<typename T>
class BlockingQueue: boost::noncopyable

{
public:
    typedef std::function< void(T) > ReleaseFunc;
private:
    struct QueueNode
    {
        T value;
        bool isKeyValue;
        ReleaseFunc releaseFunc;
    };
public:
    BlockingQueue(int capacity):
            capacity_(capacity),
            mutex_(),
            queue_(),
            forcewake_(false),
            needOverlay_(false)
    {};

    int size(){
        return queue_.size();
    }

    bool full()
    {
        return size() == capacity_;
    }

    bool empty(){
        return queue_.empty();
    }

    void forcewake() {
        forcewake_ = true;
        notEmpty_.notify_all();
        notFull_.notify_all();
    }

    void overlay(bool flag = true) {
        needOverlay_ = flag;
    }

    template<
            typename U = T,
            typename TypeMustBeT = std::enable_if<std::is_same<std::remove_reference<U>, T>::value>
    >
    bool push(U&& value, bool isKeyValue = false, ReleaseFunc releaseFunc = nullptr) {
        std::unique_lock<std::mutex> lock_(mutex_);

        if(needOverlay_ && full())
        {
            for(auto it = queue_.begin(); it != queue_.end(); it++)
            {
                if(it->isKeyValue == false)
                {
                    if(it->releaseFunc)
                        it->releaseFunc(it->value);
                    queue_.erase(it);
                    break;
                }
            }
        }


        notFull_.wait(lock_, [this]{ return ((int)queue_.size() < capacity_) || forcewake_;});
        forcewake_ = false;
        if((int)queue_.size() < capacity_)
        {
            QueueNode node;
            node.value = std::forward<U>(value);
            node.isKeyValue = isKeyValue;
            node.releaseFunc = releaseFunc;

            queue_.push_back(node);
            notEmpty_.notify_one();
            return true;
        }
        return false;
    }

    T take() {
        std::unique_lock<std::mutex> lock_(mutex_);
        notEmpty_.wait(lock_, [this]{return (queue_.size() > 0) || forcewake_;});
        forcewake_ = false;
        if(queue_.size() > 0)
        {
//            T&& value = std::move(queue_.front());
            QueueNode value = queue_.front();
            queue_.pop_front();
            notFull_.notify_one();
//            return std::move(value);
            return value.value;
        }
        return T();
    }

    T front() {
        if(queue_.size() > 0)
        {
            QueueNode value = queue_.front();
            return value.value;
        }
        return T();
    }

private:
    int capacity_;
    std::mutex mutex_;
    std::condition_variable notFull_;
    std::condition_variable notEmpty_;
    std::deque<QueueNode> queue_;
    bool forcewake_;
    bool needOverlay_;
};

#endif // BLOCKINGQUEUE

