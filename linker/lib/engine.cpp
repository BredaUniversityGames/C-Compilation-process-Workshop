#include "engine.hpp"

[[nodiscard]] engine_t create() noexcept{
    engine_t e{};
    e.world.entities.items = new entity_t[100];
    e.world.entities.capacity = 100;
    return e;
}

entity_t engine_t::spawn(){
    if(world.entities.num_items == world.entities.capacity){
        // magic
    }else{
        world.entities.items[world.entities.num_items] = {world.entities.num_items};
    }
    return {world.entities.num_items};
}

void engine_t::update(double dt){
    //...
}
