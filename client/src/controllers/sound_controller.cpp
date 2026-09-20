//
// Created by arch on 9/19/26.
//

#include "sound_controller.hpp"

SoundController::SoundController() : volume_(0.2)
{
    type_ = new QSoundEffect();
    type_->setSource(QUrl::fromLocalFile((":/assets/sounds/type.wav")));
    type_->setVolume(volume_);

    click_ = new QSoundEffect();
    click_->setSource(QUrl::fromLocalFile(":/assets/sounds/click.wav"));
    click_->setVolume(volume_);

    notification_ = new QSoundEffect();
    notification_->setSource(QUrl::fromLocalFile(":/assets/sounds/notification.wav"));
    notification_->setVolume(volume_);
}

std::expected<void, err::Error> SoundController::playSound(const SoundType& sound) const
{
    if (volume_ == 0)
    {
        return {};
    }
    switch (sound) {
        case SoundType::Type: {
            type_->setVolume(volume_);
            type_->play();
            return {};
        }
        case SoundType::Click: {
            type_->setVolume(volume_);
            click_->play();
            return {};
        }
        case SoundType::Notification: {
            type_->setVolume(volume_);
            notification_->play();
            return {};
        }
        default: {
            return std::unexpected{err::Error{.type = err::SoundError::SoundNotExist, .message = "Sound not exists"}};
        }
    }
}

std::uint32_t SoundController::getVolume() const
{
    return static_cast<std::uint32_t>(volume_ * 100);
}

void SoundController::setVolume(std::uint32_t volume)
{
    volume_ = static_cast<std::float_t>(volume) / 100;
}
