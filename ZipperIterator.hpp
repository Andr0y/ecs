/*
** EPITECH PROJECT, 2024
** ecs
** File description:
** ZipperIterator
*/

#ifndef ZIPPERITERATOR_HPP_
#define ZIPPERITERATOR_HPP_

#include <tuple>

template <class... Containers>
class ZipperIterator {
public:
    template <class Container>
    using iterator_t = decltype(std::begin(std::declval<Container&>()));

    template <class Container>
    using it_reference_t = typename iterator_t<Container>::reference;

    using value_type = std::tuple<std::size_t, it_reference_t<Containers>...>;
    using reference = value_type;
    using iterator_category = std::forward_iterator_tag;
    using iterator_tuple = std::tuple<iterator_t<Containers>...>;
    using pred_type = std::function<bool(const value_type&)>;

    ZipperIterator(const iterator_tuple& it_tuple, const iterator_tuple& end_tuple, std::size_t max)
        : _current(it_tuple), _end(end_tuple), _max(max), _idx(0) {printf("1");}

    ZipperIterator(const iterator_tuple& it_tuple, const iterator_tuple& end_tuple, std::size_t max, pred_type&& pred)
        : _current(it_tuple), _end(end_tuple), _max(max), _idx(0), _pred(std::move(pred)) {printf("2");}

    ZipperIterator(const ZipperIterator& z)
        : _current(z._current), _end(z._end), _max(z._max), _idx(z._idx), _pred(z._pred) {printf("3");}

    ~ZipperIterator() = default;

    auto operator ++() -> ZipperIterator & {
        std::cout << "++\n";
        incr_all(_seq);
        return *this; 
    };

    auto operator ++(int) -> ZipperIterator {
        std::cout << "++2\n";
        ZipperIterator tmp(*this);
        ++(*this);
        return tmp;
    };

    auto operator *() -> value_type {
        std::cout << "()" << std::endl;
        return to_value(_seq);
    };

    auto operator ->() -> value_type {
        std::cout << "->" << std::endl;
        return to_value(_seq);
    };

    auto operator ==(const ZipperIterator <Containers ...> & rhs ) -> bool {
        std::cout << "==" << std::endl;
        return _current == rhs._current;
    } ;

    auto operator !=(const ZipperIterator <Containers ...> & rhs ) -> bool {
        std::cout << "!=" << std::endl;
        return !(*this == rhs);
    };

private:
    template <std::size_t... Is>
    auto incr_all ( std :: index_sequence < Is ... >) -> void {
        std::cout << "incr_all" << std::endl;
        (++std::get<Is>(_current), ...);
        _idx++;

        if (_pred) {
            while (_idx < _max && !(*_pred)(to_value(_seq))) {
                (++std::get<Is>(_current), ...);
                _idx++;
            }
        }

        if (!(... && (std::get<Is>(_current) != std::get<Is>(_end)))) {
            _idx = _max;
        }
    };

    template <std::size_t... Is>
    auto all_set ( std :: index_sequence < Is ... >) -> bool {
        std::cout << "all_set" << std::endl;
        return (... && (std::get<Is>(_current) != std::get<Is>(_end)));
    };

    template <std::size_t... Is>
    auto to_value ( std :: index_sequence < Is ... >) -> value_type {
        std::cout << "to_value" << std::endl;
        return std::tie(_idx, *std::get<Is>(_current)...);
    } 

    iterator_tuple _current;
    iterator_tuple _end;
    std::size_t _max;
    std::size_t _idx;
    std::optional<pred_type> _pred;
    static constexpr std :: index_sequence_for < Containers ... > _seq {};
};

#endif /* !ZIPPERITERATOR_HPP_ */
