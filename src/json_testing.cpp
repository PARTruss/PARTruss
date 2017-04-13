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
    std::cout << j;
    while(json::iterator itr = j.Vertices.start; itr != j.Vertices.end; itr++){
        std::cout << *itr;
    }
    return 1;
}
