//
// Created by arch on 9/19/26.
//

#ifndef PASSWORD_MANAGER_SOUND_CONTROLLER_HPP
#define PASSWORD_MANAGER_SOUND_CONTROLLER_HPP
#include <expected>
#include <QSoundEffect>
#include <unordered_map>

#include "domain/error/error.hpp"

enum class SoundType {
    Input,
    Notification,
    Click,
};


class SoundController {
public:
    explicit SoundController() = default;
    static std::expected<void, err::Error> playSound(const SoundType& sound);
};


#endif //PASSWORD_MANAGER_SOUND_CONTROLLER_HPP
