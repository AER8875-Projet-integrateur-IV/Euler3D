/**
 * @file StackSet.cpp
 * @author Abraham Bhérer-Constant (abraham.bconstant@gmail.com)
 * @date 2021-02-26
 * 
 * 
 */
#pragma once

#include <set>
#include <list>
#include <stdexcept>
#include <iterator> 
#include <cstddef>
#include <iostream>

/**
 * @brief Container with unique items. Items can be taken from any position
 *      within the container and brought back to the first position.
 * 
 * @tparam T 
 */
template<typename T> class StackSet
{
private:
    using order_type = std::list<const T*>;
    order_type _order;
    std::set<T> _items;

public:

    struct Iterator
    {
        using iterator_category =   std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;   // should be changed

        Iterator(typename order_type::iterator lsIter) : 
            _lsIter(lsIter) {
        }

        T operator*() const { 
            T val = **_lsIter;
            return val;
        }
                
        /**
         * @brief prefix increment
         * 
         * @return Iterator& 
         */
        Iterator& operator++() {
            _lsIter++; 
            return *this; 
        }  

        /**
         * @brief postfix increment
         * 
         * @return Iterator 
         */
        Iterator operator++(int) { 
            Iterator tmp = *this; 
            ++this; 
            return tmp; 
        }
        friend bool operator== (const Iterator& a, const Iterator& b) { return a._lsIter == b._lsIter; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a._lsIter != b._lsIter; };   
    private:
        typename order_type::iterator _lsIter;
    };

    template<class InputIterator,
            class = std::enable_if_t<
                        std::is_base_of<
                            std::input_iterator_tag,
                            typename std::iterator_traits<InputIterator>::iterator_category
                        >{}
                    >
            >
    StackSet(InputIterator first,InputIterator last){
        _items = std::set(first, last);
        for(auto i = _items.begin(); i != _items.end() ; i++){
            const T* val= &*i;
            _order.push_back(val);
        }
    }

    /**
     * @brief Move param to beggining of the StackSet
     * 
     * @param param item to move to the beggining. Must already exist in the 
     *          StackSet
     */
    void toFront(T& param){
        auto it = _items.find(param);
        auto end = _items.end();
        
        if(it == end){
            throw std::invalid_argument("param does not exist in StackSet");
        }
        const T* val= &*it;
        _order.remove(val);
        _order.push_front(val);
    }

    T operator[](int idx) const {
        auto it = _order.begin();
        std::advance(it, idx);
        T val = **it;
        return val; 
    }    

    Iterator begin() { return Iterator(_order.begin()); }
    Iterator end()   { return Iterator(_order.end()); } 


};
