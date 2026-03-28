#pragma once

#include "Includes.hpp"

class LinkCell : public CCNode {

private:

    GJGameLevel* m_level = nullptr;
    GJGameLevel* m_linkTo = nullptr;

    LinkCell(GJGameLevel*, GJGameLevel*);

    bool init(bool);

public:

    static LinkCell* create(GJGameLevel*, GJGameLevel*, bool);

};