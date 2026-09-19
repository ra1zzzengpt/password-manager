//
// Created by arch on 9/19/26.
//

#include "sound_controller.hpp"

std::expected<void, err::Error> SoundController::playSound(const SoundType& sound) {
    switch (sound) {
        case SoundType::Input: {
            QSoundEffect* type_sound = new QSoundEffect();
            type_sound->setSource(QUrl::fromLocalFile((":/assets/sounds/type.wav")));
            type_sound->setVolume(0.2);
            type_sound->play();
            return {};
        }
        case SoundType::Click: {
            QSoundEffect* click_sound = new QSoundEffect();
            click_sound->setSource(QUrl::fromLocalFile(":/assets/sounds/click.wav"));
            click_sound->setVolume(0.2);
            click_sound->play();
            return {};
        }
        case SoundType::Notification: {
            QSoundEffect* notification_sound = new QSoundEffect();
            notification_sound->setSource(QUrl::fromLocalFile(":/assets/sounds/notification.wav"));
            notification_sound->setVolume(0.2);
            notification_sound->play();
            return {};
        }
        default: {
            return std::unexpected{err::Error{.type = err::SoundError::SoundNotExist, .message = "Sound not exists"}};
        }
    }
}
