#include "SparseArray.hpp"
#include <unordered_map>
#include <typeindex>
#include <any>
#include <functional>
#include <list>
#include "Entity.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include <chrono>
#include <iostream>

class ECS {
public :
    using system_type = std :: function < void ( ECS &) >;
    using systems_type = std :: vector < system_type >;
    using freset_type = std::function <void(const Entity &)>;
    sf::RenderWindow window;
    using callback_type = std::function <void(void)>;
    template <class Event>
    using system_type_event = std::function <void(ECS&, const Event&)>;
    template <class Event>
    using systems_type_event = std::vector <system_type_event<Event>>;
    using event_type = std::any;

public :
    ECS () {
        _nb_entities = 0;
        window.create(sf::VideoMode(800, 600), "My window");
        window.setVerticalSyncEnabled(true);
    };
    ~ ECS () = default ;

    struct Position {
        float x;
        float y;

        Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
    };

    struct Drawable {
        sf::RectangleShape shape;

        Drawable(float size = 50.0f, sf::Color color = sf::Color::Red) {
            shape.setSize(sf::Vector2f(size, size));
            shape.setFillColor(color);
        }
    };

    struct Velocity {
        float vx;
        float vy;

        Velocity(float vx = 0.0f, float vy = 0.0f) : vx(vx), vy(vy) {}
    };

    struct Controllable {
        float speed;

        Controllable(float speed = 0.1f) : speed(speed) {}
    };

    template <class Component >
    auto register_component() -> SparseArray <Component> & {
        auto &comp_array = _components_arrays[std::type_index(typeid(Component))];
        if (!comp_array.has_value()) {
            comp_array = std::make_any<SparseArray<Component>>();
            _freset_entity_components.push_back([this](const Entity& e) {
                auto& array = ECS::get_components<Component>();
                array.erase(e.nb);
            });
        }
        return std::any_cast<SparseArray<Component>&>(comp_array);
    };

    template <class Component >
    auto get_components () -> SparseArray <Component> & {
        auto &comp_array = _components_arrays[std::type_index(typeid(Component))];
        return std::any_cast<SparseArray<Component>&>(comp_array);
    };

    template <class Component >
    auto get_components () const -> const SparseArray <Component> &{
        const auto &comp_array = _components_arrays.at(std::type_index(typeid(Component)));
        return std::any_cast<const SparseArray<Component>&>(comp_array);
    };

    auto spawn_entity () -> Entity {
        if (_dead_entities.empty()) {
            Entity e(_nb_entities);
            _nb_entities++;
            return e;
        }
        Entity e = _dead_entities.front();
        _dead_entities.pop_front();
        return e;
    };

    auto entity_from_index (std::size_t idx) -> Entity {
        if (idx > _nb_entities)
            throw std::runtime_error("entity_from_index() out of range ");
        return Entity(idx);
    };

    auto kill_entity ( const Entity & entity ) -> void {
        for (auto & element : _freset_entity_components) {
            element(entity);
        }
        _dead_entities.push_back(entity);
    };

    template <typename Component >
    auto add_component ( Entity const & to , Component && c ) ->
        typename SparseArray <Component>::reference_type
    {
        auto &comp_array = get_components<Component>();
        return comp_array.insert_at(to.nb, c);
    };

    template <typename Component , typename ... Params >
    auto emplace_component ( Entity const & to , Params &&... p ) ->
        typename SparseArray < Component >:: reference_type
    {
        auto &comp_array = get_components<Component>();
        if (!comp_array.has_value())
            return;
        const SparseArray<Component> &comp_array_cast = std::any_cast<const SparseArray<Component>&>(comp_array);
        return comp_array_cast.emplace_at(to.nb, p ...);
    };

    template <typename Component >
    auto remove_component ( Entity const & from ) -> void {
        auto &comp = get_components<Component>();
        comp.erase(from.nb);
    };

    template <class ... Components , typename System >
    auto add_system ( System && system ) -> void {
        _systems.push_back([this, system](ECS& ecs) {
            system(ecs, ecs.get_components<Components>()...);
        });
    };

    template <class ... Components , typename System >
    auto add_system ( System & system ) -> void {
        _systems.push_back([this, system](ECS& ecs) {
            system(ecs, ecs.get_components<Components>()...);
        });
    };


    auto run_systems ( void ) -> void {
        for (auto & element : _systems) {
            element(*this);
        }
    };

    template <class Event>
    auto register_event() -> void {
        _events[std::type_index(typeid(Event))] = systems_type_event<Event>();
    }

    template <class Event>
    auto get_events() -> systems_type_event<Event>& {
        return std::any_cast<systems_type_event<Event>&>(_events[std::type_index(typeid(Event))]);
    }

    template <class Event>
    auto get_events() const -> const systems_type_event<Event>& {
        return std::any_cast<const systems_type_event<Event>&>(_events.at(std::type_index(typeid(Event))));
    }

    template <class Event, class... Components, typename System>
    auto subscribe(System&& system) -> void {
        auto& events = get_events<Event>();

        events.push_back([this, system](ECS& ecs, const Event& event) {
            system(ecs, ecs.get_components<Components>()..., event);
        });
    }

    template <class Event, class... Components, typename System>
    auto subscribe(System& system) -> void {
        auto& events = get_events<Event>();

        events.push_back([this, system](ECS& ecs, const Event& event) {
            system(ecs, ecs.get_components<Components>()..., event);
        });
    }

    template <class Event, class... Components, typename System>
    auto subscribe(const System& system) -> void {
        auto& events = get_events<Event>();

        events.push_back([this, system](ECS& ecs, const Event& event) {
            system(ecs, ecs.get_components<Components>()..., event);
        });
    }

    template <class Event>
    auto post(const Event& event) -> void {
        auto& systems = get_events<Event>();

        _callback_pool.push_back([this, systems, event = std::move(event)]() {
            for (auto& system : systems) {
                system(*this, event);
            }
        });
    }

    const callback_type& front() const {
        return _callback_pool.front();
    }

    void pop_front() {
        _callback_pool.pop_front();
    }

    bool empty() const {
        return _callback_pool.empty();
    }

private :
    std::size_t _nb_entities ;
    std::unordered_map <std::type_index, std::any> _components_arrays ;
    std::list <Entity> _dead_entities ;
    std::vector <freset_type> _freset_entity_components ;
    systems_type _systems;
    std::unordered_map <std::type_index, event_type> _events;
    std::list <callback_type> _callback_pool;
};
