#pragma once

#include "Includes.hpp"

class Manager {

public:

    int m_realAttempts = 0;
    int m_lostAttempts = 0;

    static Manager& get();

    static bool areLevelsLinked(GJGameLevel*, GJGameLevel*);
    static void linkLevels(GJGameLevel*, GJGameLevel*);
    static void unlinkLevels(GJGameLevel*, GJGameLevel*);
    static bool isLevelLinked(GJGameLevel*, std::unordered_set<int>&);
    static GJGameLevel* findEditorLevel(int);
    static GJGameLevel* getLevel(int);
    static void addLostAttempts(GJGameLevel*, int);
    static int getLostAttempts(GJGameLevel*);
    static int getAttempts(GJGameLevel*, std::unordered_set<int>&, bool);
    static int getRealAttempts(GJGameLevel*, std::unordered_set<int>&, bool);
    static std::string toLowercase(std::string);
    static void setHookEnabled(std::string_view, bool);

};