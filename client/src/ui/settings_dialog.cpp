#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QSlider>
#include <QRadioButton>
#include <QApplication>
#include <QFile>
#include <ui/settings_dialog.hpp>

SettingsDialog::SettingsDialog(MainController &controller, SoundController& sound_controller,
                               ConfigurationController& configuration_controller, QApplication& app,
                               QWidget *parent)
    : QDialog(parent), controller_(controller), sound_controller_(sound_controller),
      configuration_controller_(configuration_controller), app_(app)
{
    this->setFixedSize(600,390);
    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* top_layout = new QHBoxLayout();

    QIcon backIcon(":/assets/icons/exit.png");
    QPushButton* back_button = new QPushButton(this);
    back_button->setIcon(backIcon);

    QLabel* back_label = new QLabel("Settings",this);

    QIcon saveIcon(":assets/icons/save.png");
    QPushButton* apply_button = new QPushButton(this);
    apply_button->setIcon(saveIcon);

    top_layout->addWidget(back_button);
    top_layout->addStretch();
    top_layout->addWidget(back_label);
    top_layout->addStretch();
    top_layout->addWidget(apply_button);

    QHBoxLayout* columns_layout = new QHBoxLayout();

    QVBoxLayout* left_layout = new QVBoxLayout();

    QGroupBox* password_group = new QGroupBox("Change master-password",this);
    password_group->setAlignment(Qt::AlignCenter);

    QVBoxLayout* password_group_layout = new QVBoxLayout(password_group);

    QLineEdit* old_password = new QLineEdit(this);
    old_password->setPlaceholderText("old master-password...");
    old_password->setEchoMode(QLineEdit::Password);


    QLineEdit* new_password = new QLineEdit(this);
    new_password->setPlaceholderText("new master-password...");
    new_password->setEchoMode(QLineEdit::Password);

    QPushButton* password_button = new QPushButton("Change",this);

    password_group_layout->addWidget(old_password);
    password_group_layout->addWidget(new_password);
    password_group_layout->addWidget(password_button);

    QLabel* error = new QLabel(this);
    error->setAlignment(Qt::AlignCenter);
    error->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    error->setObjectName("error");

    // --- SFX ---

    QGroupBox* sfx_groupbox = new QGroupBox("SFX", this);
    sfx_groupbox->setAlignment(Qt::AlignCenter);

    QVBoxLayout* sfx_layout = new QVBoxLayout(sfx_groupbox);

    QHBoxLayout* volume_layout = new QHBoxLayout();

    QLabel* mini_volume = new QLabel(this);
    mini_volume->setObjectName("miniLabel");
    mini_volume->setText("Volume: " + QString::number(configuration_controller_.config().volume));

    QCheckBox* sound_enabled = new QCheckBox(this);
    sound_enabled->setText("Sound Enabled");
    sound_enabled->setChecked(sound_controller_.isSoundEnabled());

    volume_layout->addWidget(mini_volume);
    volume_layout->addWidget(sound_enabled);

    QSlider* volume_slider = new QSlider(this);
    volume_slider->setOrientation(Qt::Horizontal);
    volume_slider->setMinimum(0);
    volume_slider->setMaximum(100);
    volume_slider->setValue(static_cast<int>(configuration_controller_.config().volume));
    mini_volume->setEnabled(sound_enabled->isChecked());
    volume_slider->setEnabled(sound_enabled->isChecked());

    sfx_layout->addLayout(volume_layout);
    sfx_layout->addWidget(volume_slider);

    QVBoxLayout* right_layout = new QVBoxLayout();

    QGroupBox* theme_groupbox = new QGroupBox("Theme", this);
    theme_groupbox->setAlignment(Qt::AlignCenter);

    QVBoxLayout* theme_layout = new QVBoxLayout(theme_groupbox);

    QRadioButton* white_theme = new QRadioButton(this);
    white_theme->setIcon(QIcon(":/assets/icons/white_theme.png"));
    white_theme->setIconSize(QSize(64,64));

    QRadioButton* dark_theme = new QRadioButton(this);
    dark_theme->setIcon(QIcon(":/assets/icons/dark_theme.png"));
    dark_theme->setIconSize(QSize(64,64));

    QRadioButton* nord_theme = new QRadioButton(this);
    nord_theme->setIcon(QIcon(":/assets/icons/nord_theme.png"));
    nord_theme->setIconSize(QSize(64,64));

    theme_layout->addWidget(white_theme);
    theme_layout->addWidget(dark_theme);
    theme_layout->addWidget(nord_theme);
    const auto& current_theme = configuration_controller_.config().theme;
    if (current_theme == "white") white_theme->setChecked(true);
    else if (current_theme == "dark") dark_theme->setChecked(true);
    else if (current_theme == "nord") nord_theme->setChecked(true);

    connect(sound_enabled, &QCheckBox::toggled, [=,this](const bool state)
    {
        mini_volume->setEnabled(state);
        volume_slider->setEnabled(state);
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
        this->deleteLater();
    });

    connect(volume_slider, &QSlider::valueChanged,[mini_volume](const int volume)
    {
        mini_volume->setText("Volume: " + QString::number(volume));
    });

    connect(apply_button, &QPushButton::clicked, [=, this]
    {
        Config updated = configuration_controller_.config();
        updated.volume = static_cast<std::uint32_t>(volume_slider->value());
        if (white_theme->isChecked()) updated.theme = "white";
        else if (dark_theme->isChecked()) updated.theme = "dark";
        else if (nord_theme->isChecked()) updated.theme = "nord";

        const QString theme_path = ":/assets/" + QString::fromStdString(updated.theme) + "_theme.qss";
        QFile theme(theme_path);
        if (!theme.open(QFile::ReadOnly))
        {
            QMessageBox::warning(this, "Warning", "Can't open theme resource: " + theme_path);
            return;
        }
        const QString style_sheet = QString::fromUtf8(theme.readAll());
        if (auto result = configuration_controller_.setConfig(updated); !result)
        {
            QMessageBox::warning(this, "Warning", QString::fromStdString(result.error().message));
            return;
        }
        sound_controller_.setVolume(updated.volume);
        sound_controller_.setSoundEnabled(sound_enabled->isChecked());
        app_.setStyleSheet(style_sheet);
    });

    left_layout->addWidget(password_group);
    left_layout->addWidget(sfx_groupbox);
    left_layout->addWidget(error);

    right_layout->addWidget(theme_groupbox);

    columns_layout->addLayout(left_layout);
    columns_layout->addLayout(right_layout);
    layout->addLayout(top_layout);
    layout->addLayout(columns_layout);
}
