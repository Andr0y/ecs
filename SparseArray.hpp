/*
** EPITECH PROJECT, 2024
** ecs
** File description:
** SparseArray
*/

#pragma once

#include <vector>
#include <optional>

template <typename Component> class SparseArray {
public :
    using value_type = std::optional<Component>; // optional component type
    using reference_type = value_type &;
    using const_reference_type = const value_type &;
    using container_t = std::vector<value_type>;
    using size_type = typename container_t::size_type ;
    using iterator = typename container_t::iterator ;
    using const_iterator = typename container_t::const_iterator;

public :
    SparseArray() {};

    SparseArray (const SparseArray &other) = default;

    SparseArray ( SparseArray && other ) noexcept = default;

    ~SparseArray() = default ;

    auto operator =( const SparseArray & other ) -> SparseArray & {
        _data = other._data;
        return *this;
    };

    auto operator =( SparseArray && other ) noexcept -> SparseArray & {
        _data = std::move(other._data);
        return *this;
    };

    auto operator []( size_t idx ) -> reference_type {
        return _data[idx];
    };

    auto operator []( size_t idx ) const -> const_reference_type {
        return _data[idx];
    };

    auto begin () -> iterator {
        return _data.begin();
    };

    auto begin () const -> const_iterator {
        return _data.cbegin();
    };

    auto cbegin () const -> const_iterator {
        return _data.cbegin();
    };

    auto end () -> iterator {
        return _data.end();
    };

    auto end () const -> const_iterator {
        return _data.cend();
    };

    auto cend () const -> const_iterator {
        return _data.cend();
    };

    auto size () const -> size_type {
        return _data.size();
    };

    auto insert_at ( size_type pos , const Component & value ) -> reference_type {
        _data.insert(_data.begin() + pos, value);
        return _data[pos];
    };

    auto insert_at ( size_type pos , Component && rvalue ) -> reference_type {
        _data.insert(_data.begin() + pos, rvalue);
        return _data[pos];
    };

    template <class ... Params >
    auto emplace_at ( size_type pos , Params &&... args ) -> reference_type {
        return _data.emplace(_data.begin() + pos, args ...);
    };

    auto erase ( size_type pos ) -> void {
        _data[pos].reset();
    };

    auto clear ( void ) -> void {
        _data.clear();
    };

private :
    container_t _data ;
};
