#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QMessageBox>
#include <qslider.h>
#include <ui/settings_dialog.hpp>

SettingsDialog::SettingsDialog(MainController &controller, SoundController& sound_controller, QWidget *parent) : QDialog(parent), controller_(controller), sound_controller_(sound_controller)
{
    this->setFixedSize(600,350);
    QVBoxLayout* layout = new QVBoxLayout(this);

    QGroupBox* password_group = new QGroupBox("Change master-password",this);
    password_group->setAlignment(Qt::AlignCenter);

    QVBoxLayout* password_group_layout = new QVBoxLayout(password_group);

    QLineEdit* old_password = new QLineEdit(this);
    old_password->setPlaceholderText("old master-password...");
    old_password->setMinimumSize(QSize(450,14));
    old_password->setEchoMode(QLineEdit::Password);


    QLineEdit* new_password = new QLineEdit(this);
    new_password->setPlaceholderText("new master-password...");
    new_password->setEchoMode(QLineEdit::Password);

    password_group_layout->addWidget(old_password);
    password_group_layout->addWidget(new_password);

    QLabel* error = new QLabel(this);
    error->setAlignment(Qt::AlignCenter);
    error->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    error->setObjectName("error");

    QHBoxLayout* low_layout = new QHBoxLayout();

    QPushButton* back_button = new QPushButton("Back",this);
    QPushButton* password_button = new QPushButton("Change",this);

    low_layout->addWidget(back_button);
    low_layout->addStretch();
    low_layout->addWidget(password_button);

    // --- SFX ---

    QGroupBox* sfx_groupbox = new QGroupBox("SFX", this);
    sfx_groupbox->setAlignment(Qt::AlignCenter);

    QVBoxLayout* sfx_layout = new QVBoxLayout(sfx_groupbox);

    QHBoxLayout* volume_layout = new QHBoxLayout();

    QLabel* mini_volume = new QLabel(this);
    mini_volume->setObjectName("miniLabel");
    mini_volume->setText(("Volume: " + std::to_string(sound_controller_.getVolume())).c_str());

    QCheckBox* sound_enabled = new QCheckBox(this);
    sound_enabled->setText("Sound Enabled");
    sound_enabled->setChecked(true);

    volume_layout->addWidget(mini_volume);
    volume_layout->addWidget(sound_enabled);

    QSlider* volume_slider = new QSlider(this);
    volume_slider->setOrientation(Qt::Horizontal);
    volume_slider->setMinimum(0);
    volume_slider->setMaximum(100);
    volume_slider->setValue(static_cast<std::int32_t>(sound_controller_.getVolume()));

    sfx_layout->addLayout(volume_layout);
    sfx_layout->addWidget(volume_slider);

    connect(sound_enabled, &QCheckBox::toggled, [=,this](const bool state)
    {
        sound_controller_.setSoundEnabled(state);
        if (!state)
        {
            mini_volume->setEnabled(false);
            volume_slider->setEnabled(false);
        } else
        {
            mini_volume->setEnabled(true);
            volume_slider->setEnabled(true);
        }
    });

    connect(old_password, &QLineEdit::textChanged,[this]
    {
        if (auto res = sound_controller_.playSound(SoundType::Type); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
    });

    connect(new_password, &QLineEdit::textChanged,[this]
    {
        if (auto res = sound_controller_.playSound(SoundType::Type); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
    });

    connect(new_password, &QLineEdit::returnPressed, [=,this]()
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        error->clear();
        if (const std::expected<void, err::Error> set_res = controller_.changeMasterPassword(old_password->text().toStdString(),new_password->text().toStdString()); !set_res.has_value())
        {
            error->setText(QString{set_res.error().message.c_str()});
        } else
        {
            new_password->clear();
            old_password->clear();
        }
    });

    connect(password_button, &QPushButton::clicked,new_password, &QLineEdit::returnPressed);


    connect(back_button, &QPushButton::clicked,[this]
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        this->close();
    });

    connect(volume_slider, &QSlider::valueChanged,[=,this]
    {
        mini_volume->setText(("Volume: " + std::to_string(sound_controller_.getVolume())).c_str());
        sound_controller_.setVolume(volume_slider->value());
    });

    layout->addStretch();
    layout->addWidget(password_group);
    layout->addWidget(sfx_groupbox);
    layout->addWidget(error);
    layout->addLayout(low_layout);
    layout->addStretch();
}
