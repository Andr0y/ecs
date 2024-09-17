/*
** EPITECH PROJECT, 2024
** ecs
** File description:
** Zipper
*/

#ifndef ZIPPER_HPP_
#define ZIPPER_HPP_

#include "ZipperIterator.hpp"
#include <optional>
#include <algorithm>

template <class ... Containers > 
class Zipper {
public :
    using iterator = ZipperIterator < Containers ... >;
    using iterator_tuple = typename iterator :: iterator_tuple ;
    using value_type = typename iterator :: value_type ;
    using pred_type = typename iterator :: pred_type ;

public :
    Zipper ( Containers &... cs )
        : _begin(_compute_begin(cs...)), _end(_compute_end(cs...)),
            _size(_compute_size(cs...)) {}

    Zipper ( pred_type && pred , Containers &... cs )
        : _begin(_compute_begin(cs...)), _end(_compute_end(cs...)),
            _size(_compute_size(cs...)), _pred(std::move(pred)) {}

    ~ Zipper () = default ;

    auto begin () -> iterator {
        if (_pred)
            return iterator(_begin, _end, _size, std::move(*_pred));
        else
            return iterator(_begin, _end, _size);
    };

    auto end () -> iterator {
        return iterator(_end, _end, _size);
    };

private :
    // helper function to know the maximum index of our iterators .
    static auto _compute_size ( Containers &... containers ) -> std :: size_t {
        return std::min({containers.size()...});
    };
    // helper function to compute an iterator_tuple that will allow us to build ourend iterator .
    static auto _compute_end ( Containers &... containers ) -> iterator_tuple {
        return std::make_tuple(containers.end()...);
    };

    static auto _compute_begin ( Containers &... containers ) -> iterator_tuple {
        return std::make_tuple(containers.begin()...);
    };

    private :
    iterator_tuple _begin ;
    iterator_tuple _end ;
    std :: size_t _size ;
    std :: optional < pred_type > _pred ;
};

#endif /* !ZIPPER_HPP_ */
