#include "SoundManager.h"
#include "fmod.hpp"
#pragma comment(lib, "fmod_vc.lib") 
SoundManager* SoundManager::m_pInst = nullptr;

void SoundManager::Init()
{
    FMOD::System_Create(&m_system);
    m_system->init(32, FMOD_INIT_NORMAL, nullptr);
}

void SoundManager::Update()
{
    if (m_system)
        m_system->update();
}

void SoundManager::Release()
{
    for (auto& pair : m_sounds)
        pair.second->release();
    m_sounds.clear();

    StopBGM();

    if (m_system != nullptr)
    {
        m_system->close();
        m_system->release();
        m_system = nullptr;
    }
}

void SoundManager::Load(const std::string& key, const std::string& filePath)
{
    if (m_system == nullptr)
        return;

    FMOD::Sound* sound = nullptr;
    m_system->createSound(filePath.c_str(), FMOD_DEFAULT, nullptr, &sound);
    if (sound != nullptr)
        m_sounds[key] = sound;
}

void SoundManager::Play(const std::string& key)
{
    if (m_system == nullptr)
        return;

    auto it = m_sounds.find(key);
    if (it == m_sounds.end())
        return;

    m_system->playSound(it->second, nullptr, false, nullptr);
}

void SoundManager::PlayBGM(const std::string& filePath)
{
    if (m_system == nullptr)
        return;

    StopBGM();
    m_system->createStream(filePath.c_str(), FMOD_LOOP_NORMAL | FMOD_2D, nullptr, &m_bgm);
    m_system->playSound(m_bgm, nullptr, false, &m_bgmCh);
}

void SoundManager::StopBGM()
{
    if (m_bgmCh != nullptr)
    {
        m_bgmCh->stop();
        m_bgmCh = nullptr;
    }

    if (m_bgm != nullptr)
    {
        m_bgm->release();
        m_bgm = nullptr;
    }
}