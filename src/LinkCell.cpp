#include "LinkCell.hpp"
#include "Manager.hpp"

LinkCell* LinkCell::create(GJGameLevel* level, GJGameLevel* linkTo, bool isEditor) {
    auto ret = new LinkCell(level, linkTo);

    if (ret->init(isEditor)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

LinkCell::LinkCell(GJGameLevel* level, GJGameLevel* linkTo) :
    m_level(level),
    m_linkTo(linkTo) {}

bool LinkCell::init(bool isEditor) {
    this->setContentSize({195, 26});

    auto menu = CCMenu::create();
    menu->setPosition({0, 0});

    this->addChild(menu);

    auto bg = NineSlice::create("square02b_001.png");
    bg->setContentSize(CCSize{185, 26} / 0.5f);
    bg->setScale(0.5f);
    bg->setPosition(getContentSize() / 2.f);
    bg->setColor({0, 0, 0});
    bg->setOpacity(31);

    this->addChild(bg);

    auto icon = CCSprite::createWithSpriteFrameName(isEditor ? "GJ_hammerIcon_001.png" : "GJ_starsIcon_001.png");
    icon->setAnchorPoint({0.f, 0.5f});
    icon->setPosition({9, getContentHeight() / 2.f});
    icon->setScale(0.6);

    this->addChild(icon);

    std::string levelName = m_level->m_levelName;

    auto lbl = CCLabelBMFont::create(levelName.c_str(), "bigFont.fnt");
    lbl->setAnchorPoint({0.f, 0.5f});
    lbl->limitLabelWidth(132, 0.4f, 0.f);
    lbl->setPosition({icon->getScaledContentWidth() + 12, getContentHeight() / 2.f});

    this->addChild(lbl);

    auto toggler = CCMenuItemExt::createToggler(
        CCSprite::createWithSpriteFrameName("gj_linkBtnOff_001.png"),
        CCSprite::createWithSpriteFrameName("gj_linkBtn_001.png"),
        [this](CCMenuItemToggler* toggler) {
            if (!toggler->isToggled()) {
                Manager::linkLevels(m_level, m_linkTo);
            } else {
                Manager::unlinkLevels(m_level, m_linkTo);
            }
        }
    );
    toggler->toggle(Manager::areLevelsLinked(m_level, m_linkTo));
    toggler->setScale(0.675f);
    toggler->setPosition({174, getContentHeight() / 2.f});

    menu->addChild(toggler);

    return true;
}