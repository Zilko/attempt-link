#pragma once

#include "Includes.hpp"

#include <Geode/ui/Button.hpp>

class FiltersPopup : public Popup {

private:

    Function<void(Filter)> m_onClose;

    Button* m_plusButton = nullptr;

    CCMenu* m_difficultyMenu = nullptr;

    CCMenuItemToggler* m_savedToggle = nullptr;
    CCMenuItemToggler* m_createdToggle = nullptr;
    CCMenuItemToggler* m_ratedToggle = nullptr;

    std::vector<CCMenuItemToggler*> m_difficultyToggles;
    std::vector<CCMenuItemToggler*> m_durationToggles;

    int m_demon = 0;

    FiltersPopup(Function<void(Filter)>);

    bool init(Filter);

    void onClose(CCObject*) override;

    void updateDemonToggle();

    CCMenuItemToggler* makeDifficultyToggle(int, bool, GJDifficultyName = GJDifficultyName::Short);
    CCMenuItemToggler* makeDurationToggle(ZStringView, bool);

public:

    static FiltersPopup* create(Filter, Function<void(Filter)>);

};