#include "json.hpp"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

// for convenience
using json = nlohmann::json;


int main(){
    std::ifstream i("data/trussDat.json");
    json j;
    i >> j;
    std::cout << j << '\n' << '\n';
    for (json::iterator itr = j["Vertices"].begin(); itr != j["Vertices"].end(); itr++){
        std::cout << *itr << '\n';
    }
    return 1;
}
