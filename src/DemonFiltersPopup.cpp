#include "DemonFiltersPopup.hpp"

DemonFiltersPopup::DemonFiltersPopup(Function<void(int)> onClose)
    : m_onClose(std::move(onClose)) {}

DemonFiltersPopup* DemonFiltersPopup::create(int demon, Function<void(int)> onClose) {
    auto ret = new DemonFiltersPopup(std::move(onClose));

    if (ret->init(demon)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool DemonFiltersPopup::init(int demon) {
    Popup::init(300, 119);

    setTitle("Demon Filter");

    auto bg = NineSlice::create("square02b_001.png");
    bg->setColor({0, 0, 0});
    bg->setOpacity(86);
    bg->setContentSize({280, 63});
    bg->setPosition({m_size.width / 2.f, 49});

    m_mainLayer->addChild(bg);

    auto menu = CCMenu::create();
    menu->ignoreAnchorPointForPosition(false);
    menu->setAnchorPoint({0.5f, 0.5f});
    menu->setContentSize({280, 63});
    menu->setPosition({m_size.width / 2.f, 51});
    menu->setLayout(
        SimpleAxisLayout::create(Axis::Row)
            ->setGap(14.f)
    );

    m_mainLayer->addChild(menu);

    menu->addChild(makeDifficultyToggle(6, GJDifficultyName::Short));
    menu->addChild(makeDifficultyToggle(7, GJDifficultyName::Long));
    menu->addChild(makeDifficultyToggle(8, GJDifficultyName::Long));
    menu->addChild(makeDifficultyToggle(6, GJDifficultyName::Long));
    menu->addChild(makeDifficultyToggle(9, GJDifficultyName::Long));
    menu->addChild(makeDifficultyToggle(10, GJDifficultyName::Long));

    menu->updateLayout();

    for (int i = 0; i < m_toggles.size(); i++) {
        if (i == demon) {
            m_toggles[i]->toggle(true);
            break;
        }
    }

    return true;
}

void DemonFiltersPopup::onClose(CCObject* sender) {
    for (int i = 0; i < m_toggles.size(); i++) {
        if (m_toggles[i]->isToggled()) {
            m_onClose(i);
            break;
        }
    }
    
    Popup::onClose(sender);
}

CCMenuItemToggler* DemonFiltersPopup::makeDifficultyToggle(int difficulty, GJDifficultyName name) {
    auto sprOff = GJDifficultySprite::create(difficulty, name);
    sprOff->setColor({125, 125, 125});
    
    auto toggle = CCMenuItemExt::createToggler(
        GJDifficultySprite::create(difficulty, name),
        sprOff,
        [this](CCMenuItemToggler* toggle) {
            if (toggle->isToggled()) {
                toggle->toggle(false);
            }

            for (auto _toggle : m_toggles) {
                if (toggle != _toggle) {
                    _toggle->toggle(false);
                }
            }
        }
    );
    toggle->setScale(0.8f);

    m_toggles.push_back(toggle);

    return toggle;
}