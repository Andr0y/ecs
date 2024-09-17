#include <iostream>
#include "SparseArray.hpp"
#include "ECS.hpp"
#include "SFML/Graphics.hpp"
#include "Zipper.hpp"

void movement_system(ECS& ecs, SparseArray<ECS::Velocity> const& velocities, SparseArray<ECS::Position> & positions)
{
    //auto& velocities = ecs.get_components<ECS::Velocity>();
    //auto& positions = ecs.get_components<ECS::Position>();

    for (size_t i = 0; i < positions.size() && i < velocities.size(); i++) {
        //auto velo = velocities[i].value();
        //auto pos = positions[i].value();
        //if (!velocities[i] || !positions[i])
            //continue;
        positions[i]->x += velocities[i]->vx;
        positions[i]->y += velocities[i]->vy;
    }
}

void control_system(ECS& ecs, SparseArray<ECS::Controllable> const& controllables, SparseArray<ECS::Velocity> & velocities)
{
    //auto& controllables = ecs.get_components<Controllable>();
    //auto& velocities = ecs.get_components<Velocity>();

    for (size_t i = 0; i < controllables.size() && i < velocities.size(); i++) {
        if (!controllables[i] || !velocities[i])
            continue;
        //auto control = controllables[i].value();
        //auto veloc = velocities[i].value();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            velocities[i]->vx = (0.00f - controllables[i]->speed);
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            velocities[i]->vx = (0.00f + controllables[i]->speed);
        else
            velocities[i]->vx = 0.00f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            velocities[i]->vy = (0.00f - controllables[i]->speed);
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            velocities[i]->vy = (0.00f + controllables[i]->speed);
        else
            velocities[i]->vy = 0.00f;
    }
}

void logging_system ( ECS &r ,
SparseArray < ECS::Position > const & positions ,
SparseArray < ECS::Velocity > const & velocities)
{
    std::cout << "A";
    for ( auto &&[ i , pos , vel ] : Zipper ( positions , velocities ) )
        std :: cout << i << ": Position = { " << pos . value () . x << ", " << pos . value () . y
        << " } , Velocity = { " << vel . value () . vx << ", " << vel . value () . vy << " }" <<
        std :: endl ;
}
/*
void logging_system ( ECS &r ,
SparseArray < ECS::Position > const & positions ,
SparseArray < ECS::Velocity > const & velocities )
{
for ( size_t i = 0; i < positions . size () && i < velocities . size () ; ++ i ) {
auto const & pos = positions [ i ];
auto const & vel = velocities [ i ];
if ( pos && vel ) {
std :: cerr << i << " : Position = { " << pos . value () . x << ", "
<< pos . value () . y << " } , Velocity = { "
<< vel . value () . vx << ", " << vel . value () . vy << " }"
<< std :: endl ;
}
}
}
*/

void render_system(ECS& ecs, SparseArray<ECS::Position> const& positions, SparseArray<ECS::Drawable> const& drawables)
{
    //auto& positions = ecs.get_components<Position>();
    //auto& drawables = ecs.get_components<Drawable>();

    for (size_t i = 0; i < positions.size() && i < drawables.size(); i++) {
        if (!positions[i] || !drawables[i])
            continue;
        auto pos = positions[i].value();
        auto draw = drawables[i].value();
        draw.shape.setPosition(pos.x, pos.y);
        ecs.window.draw(draw.shape);
    }
}

void registerAllComponents(ECS& ecs)
{
    ecs.register_component<ECS::Position>();
    ecs.register_component<ECS::Drawable>();
    ecs.register_component<ECS::Controllable>();
    ecs.register_component<ECS::Velocity>();
}

Entity createSquare(ECS& ecs, float x, float y)
{
    Entity entity = ecs.spawn_entity();

    ecs.add_component(entity, ECS::Position(x, y));
    ecs.add_component(entity, ECS::Drawable(100.0f, sf::Color::Red));
    ecs.add_component(entity, ECS::Controllable(5.0f));
    ecs.add_component(entity, ECS::Velocity());
    return entity;
}

namespace timer = std::chrono ;

struct FrameEvent {
    FrameEvent (const timer::time_point <timer::steady_clock> & time_stamp) : time_stamp(time_stamp) {} ;
    ~ FrameEvent () = default ;
    timer::time_point <timer::steady_clock> time_stamp ;
};

struct QuitEvent {
    QuitEvent () = default ;
    ~ QuitEvent () = default ;
};

struct LogEvent {
    LogEvent() = default;
    ~LogEvent() = default;
};
/*
int main(void)
{
    ECS ecs;
    bool running = true;
    unsigned int frame_counter = 0;

    ecs.register_event<FrameEvent>();
    ecs.register_event<QuitEvent>();
    ecs.register_event<LogEvent>();
    registerAllComponents(ecs);

    ecs.subscribe<QuitEvent>(
        [&running](ECS&, const QuitEvent&) -> void {
            running = false;
        }
    );

    ecs.subscribe<FrameEvent>(
        [](ECS&, const FrameEvent&) -> void {
            std::cout << "Frame!" << std::endl;
        }
    );

    //ecs.subscribe<LogEvent, ECS::Position, ECS::Velocity>(logging_system);
    ecs.add_system<ECS::Position, ECS::Velocity>(logging_system);

    const auto FPS = 10;
    const timer::duration<float, std::ratio<1, FPS>> frameRate(1);
    timer::time_point<timer::steady_clock> current_time;
    timer::time_point<timer::steady_clock> previous_time(timer::steady_clock::now());
    timer::duration<float> dtime = timer::duration<float>::zero();

    while (running) {
        current_time = timer::steady_clock::now();
        dtime += current_time - previous_time;
        previous_time = current_time;

        ecs.run_systems();

        if (frame_counter > 100) {
            ecs.post(QuitEvent{});
        }

        if (dtime >= frameRate) {
            ecs.post(FrameEvent{current_time});
            ++frame_counter;
            dtime = std::chrono::duration<float>::zero();
        }

        while (!ecs.empty()) {
            auto evt = ecs.front();
            evt();
            ecs.pop_front();
        }
    }

    return 0;
}
*/

int main(void)
{
    ECS ecs;

    registerAllComponents(ecs);
    createSquare(ecs, 100, 30);
    createSquare(ecs, 67, 90);
    //createSquare(ecs, 78, 300);
    ecs.add_system<ECS::Position, ECS::Drawable>(render_system);
    ecs.add_system<ECS::Position, ECS::Velocity>(logging_system);
    ecs.add_system<ECS::Controllable, ECS::Velocity>(control_system);
    ecs.add_system<ECS::Velocity, ECS::Position>(movement_system);


    while (ecs.window.isOpen())
    {
        sf::Event event;
        while (ecs.window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                ecs.window.close();
        }
        ecs.window.clear(sf::Color::Black);
        //logging_system(ecs);
        //render_system(ecs, ecs.window);

        ecs.run_systems();
        ecs.window.display();
    }
    return 0;
}


