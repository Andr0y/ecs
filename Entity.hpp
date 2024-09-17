#include <cstddef>

class Entity {
friend class ECS;
private:
    size_t nb;

    explicit Entity(size_t siz) {
        nb = siz;
    }
};
