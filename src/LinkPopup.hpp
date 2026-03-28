#pragma once

#include "Includes.hpp"

class LinkPopup : public Popup {

private:

    GJGameLevel* m_level = nullptr;

    ScrollLayer* m_scroll = nullptr;

    Border* m_border = nullptr;

    std::string m_search;

    LinkPopup(GJGameLevel*);

    bool init() override;

    void updateScroll();

public:

    static LinkPopup* create(GJGameLevel*);

};