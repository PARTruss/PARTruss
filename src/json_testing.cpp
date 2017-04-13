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
    return 1;
}
