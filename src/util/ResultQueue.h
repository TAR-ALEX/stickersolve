#include <queue>
#include <mutex>
#include <atomic>

// Implements a thread saffe result queue that can be closed when it is done sending results.
template<class T>
class ResultQueue: protected std::queue<T>{
    std::mutex m;
    std::atomic<bool> isClosedB = false;

public:

    inline void close(){
        isClosedB = true;
    }

    inline bool isClosed(){
        return isClosedB;
    }

    inline T &front(){
        std::lock_guard<std::mutex> l (m);
        return std::queue<T>::front();
    }
    inline T &back(){
        std::lock_guard<std::mutex> l (m);
        return std::queue<T>::back();
    }

    inline bool empty() const {
        std::lock_guard<std::mutex> l (m);
        return std::queue<T>::empty();
    }
    inline std::size_t size() const{
        std::lock_guard<std::mutex> l (m);
        return std::queue<T>::size();
    }

    inline void pop(){
        std::lock_guard<std::mutex> l (m);
        std::queue<T>::pop();
    }
    inline void push(T &&__x){
        std::lock_guard<std::mutex> l (m);
        std::queue<T>::push();
    }

    template<class... _Args> inline void emplace(_Args &&...__args){
        std::lock_guard<std::mutex> l (m);
        std::queue<T>::emplace(__args);
    }

    
    //inline void swap(std::queue<T> &__q);

};

/*
(constructor)
    Construct queue (public member function )

empty
    Test whether container is empty (public member function )

size
    Return size (public member function )

front
    Access next element (public member function )

back
    Access last element (public member function )

push
    Insert element (public member function )

emplace
    Construct and insert element (public member function )

pop
    Remove next element (public member function )

swap
    Swap contents (public member function )
*/