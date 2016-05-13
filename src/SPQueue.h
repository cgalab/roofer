#ifndef SPQUEUE_H_
#define SPQUEUE_H_

/**
 * Searchable Priority Queue by extending std::priority_queue
 * SRC: http://stackoverflow.com/questions/16749723/how-i-can-find-value-in-priority-queue
 * */

#include <queue>


template<
    class T,
    class Container = std::vector<T>,
    class Compare = std::less<typename Container::value_type>
> class SPQueue : public std::priority_queue<T, Container, Compare>
{
public:
    typedef typename
        std::priority_queue<
        T,
        Container,
        Compare>::container_type::const_iterator const_iterator;

    const_iterator begin(){
    	return this->c.cbegin();
    }

    const_iterator end(){
    	return this->c.cend();
    }
};

#endif /* END SPQUEUE_H_ */
