#pragma once
#include <string>
#include <map>
#include "Defines.h"
#include "fmod.hpp"

#define SOUND SoundManager::GetInst()
class SoundManager
{
private:
    SoundManager() = default;
    ~SoundManager() = default;
public:
    static SoundManager* GetInst()
    {
        if (m_pInst == nullptr)
            m_pInst = new SoundManager;
        return m_pInst;
    }
    static void DestroyInst()
    {
        SAFE_DELETE(m_pInst);
    }
    void Init();
    void Update();
    void Release();

    void Load(const std::string& key, const std::string& filePath);
    void Play(const std::string& key);
    void PlayBGM(const std::string& filePath);
    void StopBGM();
private:
    static SoundManager* m_pInst;
private:
    FMOD::System* m_system = nullptr;
    FMOD::Channel* m_bgmCh = nullptr;
    FMOD::Sound* m_bgm = nullptr;
    std::map<std::string, FMOD::Sound*> m_sounds;
};