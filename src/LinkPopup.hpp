#pragma once

#include "Includes.hpp"

#include <Geode/ui/Button.hpp>

class LinkPopup : public Popup {

private:

    GJGameLevel* m_level = nullptr;

    ScrollLayer* m_scroll = nullptr;

    Border* m_border = nullptr;

    CCLabelBMFont* m_emptyLabel = nullptr;

    Button* m_filterBtn = nullptr;

    Filter m_filter;

    std::string m_search;

    LinkPopup(GJGameLevel*);

    bool init() override;

    bool doFilter(GJGameLevel*, bool);
    void updateScroll();
    void updateFilterButton();

public:

    static LinkPopup* create(GJGameLevel*);

};