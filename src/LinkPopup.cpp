#include "LinkPopup.hpp"
#include "LinkCell.hpp"
#include "Manager.hpp"

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

    setTitle("Link Attempts");

    auto input = TextInput::create(288.88f, "Search...");
    input->setScale(0.675f);
    input->setPosition({m_size.width / 2.f, 211});
    input->getBGSprite()->setOpacity(60);
    input->setCallback([this](const std::string& string) {
        m_search = string;
        updateScroll();
    });

    m_mainLayer->addChild(input);

    updateScroll();

    return true;
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

    auto savedLevels = GameLevelManager::get()->getSavedLevels(false, 0);

    if (savedLevels) {
        for (auto level : CCArrayExt<GJGameLevel*>(savedLevels)) {
            if (
                EditorIDs::getID(level) == EditorIDs::getID(m_level)
                || !m_search.empty() && Manager::toLowercase(level->m_levelName).find(Manager::toLowercase(m_search)) == std::string::npos
            ) {
                continue;
            }

            m_scroll->m_contentLayer->addChild(LinkCell::create(level, m_level, false));
        }
    }

    auto createdLevels = LocalLevelManager::get()->getCreatedLevels(0);

    if (createdLevels) {
        for (auto level : CCArrayExt<GJGameLevel*>(createdLevels)) {
            if (
                EditorIDs::getID(level) == EditorIDs::getID(m_level)
                || !m_search.empty() && Manager::toLowercase(level->m_levelName).find(Manager::toLowercase(m_search)) == std::string::npos
            ) {
                continue;
            }
            
            m_scroll->m_contentLayer->addChild(LinkCell::create(level, m_level, true));
        }
    }

    m_scroll->m_contentLayer->updateLayout();
    m_scroll->scrollToTop();
}