#include "engine.hpp"
#include "json.hpp"

[[noexcept]] void load(engine_t& engine, cstring level){
    json l{parse(level)};
}