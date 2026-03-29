#pragma once

#include <cvolton.level-id-api/include/EditorIDs.hpp>

using namespace geode::prelude;

struct Filter {
    bool saved = true;
    bool created = true;
    bool rated = false;
    int demon = 0;
    std::vector<int> difficulties = {};
    std::vector<int> durations = {};

    bool operator==(const Filter&) const = default;
};