#include "engine.hpp"
#include "level.hpp"

int main(){
    engine_t core{create()};
    load(core,"Test_level.json");
}