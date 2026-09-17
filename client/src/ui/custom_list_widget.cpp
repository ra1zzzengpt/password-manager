//
// Created by arch on 9/17/26.
//

#include "custom_list_widget.hpp"

#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>

QCustomListWidget::QCustomListWidget(const QIcon& icon, QWidget *parent) : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    button_ = new QPushButton(this);
    button_->setIcon(icon);

    layout->addWidget(button_);

    list_ = new QListWidget(this);
    list_->setWindowFlag(Qt::Popup);
    list_->setFocusPolicy(Qt::NoFocus);
    list_->hide();

    connect(button_, &QPushButton::clicked, this, &QCustomListWidget::toggledPopup);
    connect(list_, &QListWidget::itemClicked, this, &QCustomListWidget::itemClicked);
}

void QCustomListWidget::toggledPopup() {
    if (list_->isVisible()) {
        list_->hide();
        return;
    }
    QPoint bottomPos = button_->mapToGlobal(QPoint(0,button_->height()));

    list_->setFixedWidth(list_->width());

    list_->move(bottomPos);
    list_->show();
}

void QCustomListWidget::itemClicked(const QListWidgetItem* item) {
    const QString itemText = item->text();

    emit optionSelected(itemText);
}

void QCustomListWidget::addOption(const std::string &text) {
    list_->addItem(text.c_str());
}

void QCustomListWidget::addOptions(const std::vector<std::string>& options) {
    for (const auto& option : options) {
        list_->addItem(option.c_str());
    }
}
