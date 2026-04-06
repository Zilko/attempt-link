#include "LinkPopup.hpp"
#include "Manager.hpp"

#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/FLAlertLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <Geode/ui/Button.hpp>

$on_mod(Loaded) {
    Manager::setHookEnabled("FLAlertLayer::init", false);
}

class $modify(FLAlertLayer) {
    
    bool init(FLAlertLayerProtocol* delegate, char const* title, gd::string desc, char const* btn1, char const* btn2, float width, bool scroll, float height, float textScale) {
        std::string string = desc;

        auto pos = string.find("Total Attempts</c>: ");

        if (pos == std::string::npos) {
            return FLAlertLayer::init(delegate, title, desc, btn1, btn2, width, scroll, height, textScale);
        }
        
        pos += 20;

        auto end = pos;
        while (end < string.size() && std::isdigit(string[end])) {
            end++;
        }

        auto& m = Manager::get();

        string.replace(
            pos,
            end - pos,
            m.m_lostAttempts > 0 ? fmt::format("{} ({} lost)", m.m_realAttempts, m.m_lostAttempts)
            : numToString(m.m_realAttempts)
        );

        return FLAlertLayer::init(delegate, title, string, btn1, btn2, width, scroll, height, textScale);
    }

};

class $modify(LevelInfoLayer) {

    static void onModify(auto& self) {
        if (!self.setHookPriorityAfterPost("LevelInfoLayer::init", "camila314.pathfinder")) {
            geode::log::warn("Failed to set hook priority.");
        }
    }

    void onLevelInfo(CCObject* sender) {
        if (Manager::getSetting<"disable", bool>()) {
            return LevelInfoLayer::onLevelInfo(sender);
        }

        auto& m = Manager::get();
        auto set = std::unordered_set<int>{};

        m.m_lostAttempts = Manager::getRealAttempts(m_level, set, true);

        if (!Manager::isLevelLinked(m_level, set) && m.m_lostAttempts <= 0) {
            LevelInfoLayer::onLevelInfo(sender);
            return;
        }
        
        set = {};

        m.m_realAttempts = Manager::getRealAttempts(m_level, set, false);

        Manager::setHookEnabled("FLAlertLayer::init", true);

        LevelInfoLayer::onLevelInfo(sender);

        Manager::setHookEnabled("FLAlertLayer::init", false);
    }

    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelInfoLayer::init(p0, p1)) {
            return false;
        }

        auto menu = getChildByID("other-menu");

        if (!menu) {
            return true;
        }

        auto refBtn = menu->getChildByID("list-button");

        if (auto btn = menu->getChildByID("pathfinder-button")) {
            refBtn = btn;
        }

        if (!refBtn) {
            return true;
        }

        auto btn = Button::createWithSpriteFrameName("gj_linkBtn_001.png", [this](Button*) {
            LinkPopup::create(m_level)->show();
        });
        btn->setID("button"_spr);
        btn->setScale(0.975f);
        btn->setPosition(refBtn->getPosition() + CCPoint{0, 16 + refBtn->getScaledContentHeight() / 2.f});

        menu->addChild(btn);

        return true;
    }

};

class $modify(EditLevelLayer) {

    void onLevelInfo(CCObject* sender) {
        if (Manager::getSetting<"disable", bool>()) {
            return EditLevelLayer::onLevelInfo(sender);
        }

        auto& m = Manager::get();
        auto set = std::unordered_set<int>{};

        m.m_lostAttempts = Manager::getRealAttempts(m_level, set, true);

        if (!Manager::isLevelLinked(m_level, set) && m.m_lostAttempts <= 0) {
            EditLevelLayer::onLevelInfo(sender);
            return;
        }
        
        set = {};

        m.m_realAttempts = Manager::getRealAttempts(m_level, set, false);

        Manager::setHookEnabled("FLAlertLayer::init", true);

        EditLevelLayer::onLevelInfo(sender);

        Manager::setHookEnabled("FLAlertLayer::init", false);
    }

    bool init(GJGameLevel* p0) {
        if (!EditLevelLayer::init(p0)) {
            return false;
        }

        auto menu = getChildByID("info-button-menu");

        if (!menu) {
            return true;
        }

        auto refBtn = menu->getChildByID("guidelines-button");

        if (!refBtn) {
            return true;
        }

        auto btn = Button::createWithSpriteFrameName("gj_linkBtn_001.png", [this](Button*) {
            LinkPopup::create(m_level)->show();
        });
        btn->setID("button"_spr);
        btn->setScale(1.3f);
        btn->setPosition(refBtn->getPosition() + CCPoint{0, 41});

        menu->addChild(btn);

        return true;
    }

};

class $modify(PlayLayer) {

    void resetLevel() {
        if (!Manager::getSetting<"count-lost-attempts", bool>()) {
            return PlayLayer::resetLevel();
        }

        auto prevAttempts = m_level->m_attempts.value();

        PlayLayer::resetLevel();

        auto diff = m_level->m_attempts.value() - prevAttempts;

        if (diff > 0) {
            Manager::addLostAttempts(m_level, diff);
        }
    }

};