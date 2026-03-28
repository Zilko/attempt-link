#include "Manager.hpp"

Manager& Manager::get() {
    static Manager instance;
    return instance;
}

bool Manager::areLevelsLinked(GJGameLevel* level1, GJGameLevel* level2) {
    auto id1 = EditorIDs::getID(level1);
    auto id2 = EditorIDs::getID(level2);
    auto links = Mod::get()->getSavedValue<matjson::Value>("links");

    for (const auto& obj : links) {
        auto objId1 = obj[0].asInt().unwrapOr(0);
        auto objId2 = obj[1].asInt().unwrapOr(0);

        if (
            (id1 == objId1 && id2 == objId2)
            || (id1 == objId2 && id2 == objId1)
        ) {
            return true;
        }
    }

    return false;
}

void Manager::linkLevels(GJGameLevel* level1, GJGameLevel* level2) {
    if (areLevelsLinked(level1, level2)) {
        return;
    }

    auto id1 = EditorIDs::getID(level1);
    auto id2 = EditorIDs::getID(level2);
    auto links = Mod::get()->getSavedValue<matjson::Value>("links");

    if (!links.isArray()) {
        links = matjson::Value::array();
    }

    auto obj = matjson::Value::array();

    obj.push(id1);
    obj.push(id2);

    links.push(obj);

    Mod::get()->setSavedValue("links", links);
}

void Manager::unlinkLevels(GJGameLevel* level1, GJGameLevel* level2) {
    if (!areLevelsLinked(level1, level2)) {
        return;
    }

    auto id1 = EditorIDs::getID(level1);
    auto id2 = EditorIDs::getID(level2);
    auto links = Mod::get()->getSavedValue<matjson::Value>("links");
    auto newLinks = matjson::Value::array();

    for (const auto& obj : links) {
        auto objId1 = obj[0].asInt().unwrapOr(0);
        auto objId2 = obj[1].asInt().unwrapOr(0);

        if (
            (id1 == objId1 && id2 == objId2)
            || (id1 == objId2 && id2 == objId1)
        ) {
            continue;
        }

        newLinks.push(obj);
    }

    Mod::get()->setSavedValue("links", newLinks);
}

bool Manager::isLevelLinked(GJGameLevel* level, std::unordered_set<int>& ignores) {
    auto id = EditorIDs::getID(level);
    auto links = Mod::get()->getSavedValue<matjson::Value>("links");

    for (const auto& obj : links) {
        auto objId1 = obj[0].asInt().unwrapOr(0);
        auto objId2 = obj[1].asInt().unwrapOr(0);

        if (
            objId1 == id
            || objId2 == id
        ) {
            return true;
        }
    }

    return false;
}

GJGameLevel* Manager::findEditorLevel(int id) {
    for (auto level : CCArrayExt<GJGameLevel*>((LocalLevelManager::get()->getCreatedLevels(0)))) {
        if (EditorIDs::getID(level) == id) {
            return level;
        }
    }

    return nullptr;
}

GJGameLevel* Manager::getLevel(int id) {
    if (id <= 0) {
        return nullptr;
    }

    if (auto level = GameLevelManager::get()->getSavedLevel(id)) {
        return level;
    } else if (auto level = findEditorLevel(id)) {
        return level;
    }

    return nullptr;
}

void Manager::addLostAttempts(GJGameLevel* level, int attempts) {
    auto path = Mod::get()->getSaveDir() / "attempts.json";
    auto jsonRes = utils::file::readJson(path);
    auto json = jsonRes.isOk() ? jsonRes.unwrap() : matjson::Value{};
    auto id = numToString(EditorIDs::getID(level));

    if (!json.contains(id)) {
        json[id] = level->m_attempts.value();
    } else {
        json[id] = json[id].asInt().unwrapOr(0) + attempts;
    }
    
    if (!utils::file::writeString(path, json.dump(0)).isOk()) {
        log::error("Fail!");
    }
}

int Manager::getLostAttempts(GJGameLevel* level) {
    auto path = Mod::get()->getSaveDir() / "attempts.json";

    if (!std::filesystem::exists(path)) {
        return 0;
    }

    auto jsonRes = utils::file::readJson(path);
    
    if (!jsonRes.isOk()) {
        return 0;
    }

    auto json = jsonRes.unwrap();
    auto id = numToString(EditorIDs::getID(level));

    if (json.contains(id)) {
        return json[id].asInt().unwrapOr(0);
    }

    return 0;
}

int Manager::getAttempts(GJGameLevel* level, std::unordered_set<int>& ignores, bool lostAttempts) {
    if (ignores.contains(EditorIDs::getID(level))) {
        return 0;
    }

    if (lostAttempts) {
        return getLostAttempts(level) - level->m_attempts.value();
    }

    return level->m_attempts.value();
}

int Manager::getRealAttempts(GJGameLevel* level, std::unordered_set<int>& ignores, bool lostAttempts) {
    auto id = EditorIDs::getID(level);

    if (ignores.contains(id)) {
        return 0;
    }

    auto links = Mod::get()->getSavedValue<matjson::Value>("links");
    auto attempts = getAttempts(level, ignores, lostAttempts);

    ignores.insert(id);

    for (const auto& obj : links) {
        auto objId1 = obj[0].asInt().unwrapOr(0);
        auto objId2 = obj[1].asInt().unwrapOr(0);

        if (
            objId1 == id
            || objId2 == id
        ) {
            if (auto level = getLevel(objId1)) {
                attempts += getRealAttempts(level, ignores, lostAttempts);
                ignores.insert(objId1);
            }

            if (auto level = getLevel(objId2)) {
                attempts += getRealAttempts(level, ignores, lostAttempts);
                ignores.insert(objId2);
            }
        }
    }

    return attempts;
}

std::string Manager::toLowercase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    return str;
}

void Manager::setHookEnabled(std::string_view name, bool enabled) {
    for (auto hook : Mod::get()->getHooks()) {
        if (hook->getDisplayName() == name) {
            (void)(enabled ? hook->enable() : hook->disable());
            break;
        }
    }
}