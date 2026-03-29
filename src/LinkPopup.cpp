#include "LinkPopup.hpp"
#include "FiltersPopup.hpp"
#include "LinkCell.hpp"
#include "Manager.hpp"

#include <Geode/ui/GeodeUI.hpp>

LinkPopup* LinkPopup::create(GJGameLevel* level) {
    auto ret = new LinkPopup(level);

    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

LinkPopup::LinkPopup(GJGameLevel* level) :
    m_level(level) {}

bool LinkPopup::init() {
    Popup::init(230, 258);

    m_filter = Manager::getFilter();

    setTitle("Link Attempts");

    updateFilterButton();

    auto input = TextInput::create(251.8429f, "Search...");
    input->setScale(0.675f);
    input->setPosition({m_size.width / 2.f + 12.5f, 211});
    input->getBGSprite()->setOpacity(60);
    input->setCallback([this](const std::string& string) {
        m_search = string;
        updateScroll();
    });

    m_mainLayer->addChild(input);

    m_emptyLabel = CCLabelBMFont::create("Empty", "bigFont.fnt");
    m_emptyLabel->setOpacity(170);
    m_emptyLabel->setScale(0.65f);
    m_emptyLabel->setPosition({m_size.width / 2.f, 109});

    m_mainLayer->addChild(m_emptyLabel, 10);

    updateScroll();

    return true;
}

bool LinkPopup::doFilter(GJGameLevel* level, bool isEditor) {
    if (
        (isEditor && !m_filter.created)
        || (!isEditor && !m_filter.saved)
        || (m_filter.rated && level->m_stars.value() <= 0)
    ) {
        return true;
    }

    int difficulty;

    if (level->m_demon > 0) {
        difficulty = level->m_demonDifficulty > 0 ? level->m_demonDifficulty + 4 : 6;
    } else if (level->m_autoLevel) {
        difficulty = -1;
    } else if (level->m_ratings < 5) {
        difficulty = 0;
    } else {
        difficulty = level->m_ratingsSum / level->m_ratings;
    }

    if (!m_filter.difficulties.empty()) {
        if (!std::ranges::contains(m_filter.difficulties, Manager::difficultyToIndex(difficulty))) {
            return true;
        }

        if (difficulty >= 6) {
            if (
                m_filter.demon != 0
                && Manager::getDemonDifficulty(m_filter.demon) != difficulty
            ) {
                return true;
            }
        }
    }
    
    if (
        !m_filter.durations.empty()
        && !std::ranges::contains(m_filter.durations, level->m_levelLength)
    ) {
        return true;
    }

    return EditorIDs::getID(level) == EditorIDs::getID(m_level)
        || !m_search.empty() && Manager::toLowercase(level->m_levelName).find(Manager::toLowercase(m_search)) == std::string::npos;
}

void LinkPopup::updateScroll() {
    if (m_border) {
        m_border->removeFromParent();
    }
    
    m_scroll = ScrollLayer::create({195, 178}, true, true);
    m_scroll->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setGap(5.f)
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(200.f)
    );
    
    m_border = Border::create(m_scroll, {138, 77, 46, 255}, {195, 178}, {0, 0});
    m_border->ignoreAnchorPointForPosition(false);
    m_border->setPosition({m_size.width / 2.f, 104});

    m_mainLayer->addChild(m_border);

    auto filler = CCNode::create();
    filler->setContentHeight(1.25f);

    m_scroll->m_contentLayer->addChild(filler);

    auto didAdd = false;

    auto savedLevels = GameLevelManager::get()->getSavedLevels(false, 0);

    if (savedLevels) {
        for (auto level : CCArrayExt<GJGameLevel*>(savedLevels)) {
            if (doFilter(level, false)) {
                continue;
            }

            didAdd = true;

            m_scroll->m_contentLayer->addChild(LinkCell::create(level, m_level, false));
        }
    }

    auto createdLevels = LocalLevelManager::get()->getCreatedLevels(0);

    if (createdLevels) {
        for (auto level : CCArrayExt<GJGameLevel*>(createdLevels)) {
            if (doFilter(level, true)) {
                continue;
            }
            
            didAdd = true;

            m_scroll->m_contentLayer->addChild(LinkCell::create(level, m_level, true));
        }
    }

    m_scroll->m_contentLayer->updateLayout();
    m_scroll->scrollToTop();

    m_emptyLabel->setVisible(!didAdd);
}

void LinkPopup::updateFilterButton() {
    if (m_filterBtn) {
        m_filterBtn->removeFromParent();
    }

    auto spr = CCSprite::create(m_filter == Filter{} ? "GJ_button_01.png" : "GJ_button_02.png");
    
    auto spr2 = CCSprite::createWithSpriteFrameName("GJ_filterIcon_001.png");
    
    spr->addChildAtPosition(spr2, Anchor::Center);

    m_filterBtn = Button::createWithNode(spr, [this](Button*) {
        FiltersPopup::create(m_filter, [this](Filter filter) {
            m_filter = filter;
            updateScroll();
            updateFilterButton();

            Manager::saveFilter(m_filter);
        })->show();
    });
    m_filterBtn->setScale(0.48f);
    m_filterBtn->setPosition({27.8f, 211});
    
    m_mainLayer->addChild(m_filterBtn);
}