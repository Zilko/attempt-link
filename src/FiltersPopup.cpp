#include "FiltersPopup.hpp"
#include "DemonFiltersPopup.hpp"
#include "Manager.hpp"

FiltersPopup::FiltersPopup(Function<void(Filter)> onClose)
    : m_onClose(std::move(onClose)) {}

FiltersPopup* FiltersPopup::create(Filter filter, Function<void(Filter)> onClose) {
    auto ret = new FiltersPopup(std::move(onClose));

    if (ret->init(filter)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool FiltersPopup::init(Filter filter) {
    Popup::init(300, 254);
    
    m_demon = filter.demon;

    setTitle("Filters");

    auto menu = CCMenu::create();
    menu->ignoreAnchorPointForPosition(false);
    menu->setAnchorPoint({0.5f, 0.5f});
    menu->setContentSize({280, 35});
    menu->setPosition({m_size.width / 2.f, 193});
    menu->setLayout(
        SimpleAxisLayout::create(Axis::Row)
            ->setGap(5.f)
    );
    
    m_mainLayer->addChild(menu);

    m_savedToggle = CCMenuItemExt::createToggler(
        CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
        CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
        [](CCMenuItemToggler*) {}
    );
    m_savedToggle->toggle(filter.saved);
    m_savedToggle->setScale(0.8f);

    menu->addChild(m_savedToggle);

    auto lbl = CCLabelBMFont::create("Saved", "bigFont.fnt");
    lbl->setScale(0.5f);

    menu->addChild(lbl);

    auto gap = CCNode::create();
    gap->setContentWidth(10.f);

    menu->addChild(gap);

    m_createdToggle = CCMenuItemExt::createToggler(
        CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
        CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
        [](CCMenuItemToggler*) {}
    );
    m_createdToggle->toggle(filter.created);
    m_createdToggle->setScale(0.8f);

    menu->addChild(m_createdToggle);

    lbl = CCLabelBMFont::create("Created", "bigFont.fnt");
    lbl->setScale(0.5f);

    menu->addChild(lbl);

    menu->updateLayout();

    lbl = CCLabelBMFont::create("Difficulty", "bigFont.fnt");
    lbl->setScale(0.525f);
    lbl->setPosition({m_size.width / 2.f, 151});

    m_mainLayer->addChild(lbl);

    auto bg = NineSlice::create("square02b_001.png");
    bg->setColor({0, 0, 0});
    bg->setOpacity(86);
    bg->setContentSize({280, 56});
    bg->setPosition({m_size.width / 2.f, 110});

    m_mainLayer->addChild(bg);

    m_plusButton = Button::createWithSpriteFrameName("GJ_plus2Btn_001.png", [this](Button*) {
        DemonFiltersPopup::create(m_demon, [this](int demon) {
            m_demon = demon;
            updateDemonToggle();
        })->show();
    });
    m_plusButton->setScale(0.675f);
    m_plusButton->setPosition({206, 151});
    m_plusButton->setVisible(std::ranges::contains(filter.difficulties, 6));

    m_mainLayer->addChild(m_plusButton);

    m_difficultyMenu = CCMenu::create();
    m_difficultyMenu->ignoreAnchorPointForPosition(false);
    m_difficultyMenu->setAnchorPoint({0.5f, 0.5f});
    m_difficultyMenu->setContentSize({280, 56});
    m_difficultyMenu->setPosition({m_size.width / 2.f, 110});
    m_difficultyMenu->setLayout(
        SimpleAxisLayout::create(Axis::Row)
            ->setGap(7.5f)
    );
    
    m_mainLayer->addChild(m_difficultyMenu);

    for (int i = 0; i < 7; i++) {
        m_difficultyMenu->addChild(makeDifficultyToggle(i, std::ranges::contains(filter.difficulties, i)));
    }

    m_difficultyMenu->addChild(makeDifficultyToggle(-1, std::ranges::contains(filter.difficulties, 7)));

    m_difficultyMenu->updateLayout();

    lbl = CCLabelBMFont::create("Duration", "bigFont.fnt");
    lbl->setScale(0.525f);
    lbl->setPosition({m_size.width / 2.f, 66});

    m_mainLayer->addChild(lbl);

    bg = NineSlice::create("square02b_001.png");
    bg->setColor({0, 0, 0});
    bg->setOpacity(86);
    bg->setContentSize({280, 35});
    bg->setPosition({m_size.width / 2.f, 36});

    m_mainLayer->addChild(bg);

    menu = CCMenu::create();
    menu->ignoreAnchorPointForPosition(false);
    menu->setAnchorPoint({0.5f, 0.5f});
    menu->setContentSize({280, 35});
    menu->setPosition({m_size.width / 2.f, 36});
    menu->setLayout(
        SimpleAxisLayout::create(Axis::Row)
            ->setGap(9.f)
    );
    
    m_mainLayer->addChild(menu);

    menu->addChild(makeDurationToggle("Tiny", std::ranges::contains(filter.durations, 0)));
    menu->addChild(makeDurationToggle("Short", std::ranges::contains(filter.durations, 1)));
    menu->addChild(makeDurationToggle("Medium", std::ranges::contains(filter.durations, 2)));
    menu->addChild(makeDurationToggle("Long", std::ranges::contains(filter.durations, 3)));
    menu->addChild(makeDurationToggle("XL", std::ranges::contains(filter.durations, 4)));
    menu->addChild(makeDurationToggle("Plat.", std::ranges::contains(filter.durations, 5)));

    auto sprOff = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
    sprOff->setColor({125, 125, 125});
    
    m_ratedToggle = CCMenuItemExt::createToggler(
        CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png"),
        sprOff,
        [](CCMenuItemToggler*) {}
    );
    m_ratedToggle->toggle(filter.rated);
    m_ratedToggle->setScale(0.74f);

    menu->addChild(m_ratedToggle);

    menu->updateLayout();

    updateDemonToggle();

    return true;
}

void FiltersPopup::onClose(CCObject* sender) {
    auto filter = Filter{
        m_savedToggle->isToggled(),
        m_createdToggle->isToggled(),
        m_ratedToggle->isToggled(),
        m_demon
    };

    for (int i = 0; i < m_difficultyToggles.size(); i++) {
        if (m_difficultyToggles[i]->isToggled()) {
            filter.difficulties.push_back(i);
        }
    }

    for (int i = 0; i < m_durationToggles.size(); i++) {
        if (m_durationToggles[i]->isToggled()) {
            filter.durations.push_back(i);
        }
    }

    m_onClose(filter);

    Popup::onClose(sender);
}

void FiltersPopup::updateDemonToggle() {
    auto prevToggle = m_difficultyToggles[6];

    auto newToggle = makeDifficultyToggle(
        Manager::getDemonDifficulty(m_demon),
        prevToggle->isToggled(),
        m_demon == 0 ? GJDifficultyName::Short : GJDifficultyName::Long
    );

    m_difficultyToggles.pop_back();
    m_difficultyMenu->insertBefore(newToggle, m_difficultyToggles[7]);
    prevToggle->removeFromParent();
    m_difficultyMenu->updateLayout();
    m_difficultyToggles[6] = newToggle;
}

CCMenuItemToggler* FiltersPopup::makeDifficultyToggle(int difficulty, bool isToggled, GJDifficultyName name) {
    auto sprOff = GJDifficultySprite::create(difficulty, name);
    sprOff->setColor({125, 125, 125});
    
    auto toggle = CCMenuItemExt::createToggler(
        GJDifficultySprite::create(difficulty, name),
        sprOff,
        [this](CCMenuItemToggler* toggle) {
            if (m_difficultyToggles[6] == toggle) {
                m_plusButton->setVisible(!toggle->isToggled());
            }
        }
    );
    toggle->setScale(0.74f);
    toggle->toggle(isToggled);

    m_difficultyToggles.push_back(toggle);

    return toggle;
}

CCMenuItemToggler* FiltersPopup::makeDurationToggle(ZStringView text, bool isToggled) {
    auto lblOff = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    lblOff->setColor({125, 125, 125});
    
    auto toggle = CCMenuItemExt::createToggler(
        CCLabelBMFont::create(text.c_str(), "bigFont.fnt"),
        lblOff,
        [](CCMenuItemToggler*) {}
    );
    toggle->toggle(isToggled);
    toggle->setScale(0.35f);

    m_durationToggles.push_back(toggle);

    return toggle;
}