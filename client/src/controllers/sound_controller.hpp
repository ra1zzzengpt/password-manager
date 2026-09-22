//
// Created by arch on 9/19/26.
//

#ifndef PASSWORD_MANAGER_SOUND_CONTROLLER_HPP
#define PASSWORD_MANAGER_SOUND_CONTROLLER_HPP

#include <expected>
#include <QSoundEffect>

#include "domain/error/error.hpp"

enum class SoundType
{
    Type,
    Notification,
    Click,
};


class SoundController {
public:
    explicit SoundController(std::uint32_t volume);
    [[nodiscard]] std::expected<void, err::Error> playSound(const SoundType& sound) const;

    [[nodiscard]] std::uint32_t getVolume() const;
    [[nodiscard]] bool isSoundEnabled() const;
    void setVolume(std::uint32_t volume);

    void setSoundEnabled(bool enabled);
private:
    std::float_t volume_;
    bool soundsEnabled_;

    QSoundEffect* type_;
    QSoundEffect* notification_;
    QSoundEffect* click_;
};


#endif //PASSWORD_MANAGER_SOUND_CONTROLLER_HPP
