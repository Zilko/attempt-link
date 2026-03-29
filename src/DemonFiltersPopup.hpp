#pragma once

#include "Includes.hpp"

class DemonFiltersPopup : public Popup {

private:

    DemonFiltersPopup(Function<void(int)>);

    Function<void(int)> m_onClose;

    std::vector<CCMenuItemToggler*> m_toggles;

    bool init(int);

    void onClose(CCObject*) override;

    CCMenuItemToggler* makeDifficultyToggle(int, GJDifficultyName);

public:

    static DemonFiltersPopup* create(int, Function<void(int)>);

};