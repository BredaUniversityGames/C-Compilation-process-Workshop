#pragma once
using cstring = const char*;

template<typename T>
struct container_t{
    T* items;
    unsigned int num_items;
    unsigned int capacity;
};


struct entity_t {
    unsigned int id;
};


struct world_t{
    container_t<entity_t> entities;
};


struct engine_t{
    void update(double dt);
    entity_t spawn();
    world_t world;    
};

[[noexcept]][[nodiscard ]] engine_t create();