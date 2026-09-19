//
// Created by arch on 9/17/26.
//

#include "custom_list_widget.hpp"

#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>

QCustomListWidget::QCustomListWidget(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    button_ = new QPushButton(this);

    layout->addWidget(button_);

    list_ = new QListWidget(parent);
    list_->setWindowFlag(Qt::Popup);
    list_->setWindowFlag(Qt::FramelessWindowHint);
    list_->setFocusPolicy(Qt::StrongFocus);
    list_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list_->hide();

    connect(button_, &QPushButton::clicked, this, &QCustomListWidget::toggledPopup);
    connect(list_, &QListWidget::itemClicked,this, &QCustomListWidget::itemClicked);
    connect(list_, &QListWidget::itemActivated,this, &QCustomListWidget::itemClicked);
}

void QCustomListWidget::toggledPopup() {
    if (list_->isVisible()) {
        list_->hide();
        return;
    }
    QPoint bottomPos = button_->mapToGlobal(QPoint(0,button_->height()));

    const QFontMetrics fm(list_->font());
    int maxWidth = 0;

    for (int i = 0; i < list_->count(); ++i) {
        if (const QListWidgetItem* item = list_->item(i)) {
            if (const int itemWidth = fm.horizontalAdvance(item->text()); itemWidth > maxWidth) {
                maxWidth = itemWidth;
            }
        }
    }

    list_->setFixedWidth(maxWidth + 24 + (list_->frameWidth() * 2));

    if (list_->count() != 0)
    {
        list_->setFixedHeight(list_->sizeHintForRow(0) * list_->count());
    }

    list_->move(bottomPos);
    list_->show();
}

void QCustomListWidget::itemClicked(const QListWidgetItem* item) {
    const QString itemText = item->text();

    emit optionSelected(itemText);

    list_->hide();
}

void QCustomListWidget::addOption(const std::string &text) {
    list_->addItem(text.c_str());
}

void QCustomListWidget::addOptions(const std::vector<std::string>& options) {
    for (const auto& option : options) {
        list_->addItem(option.c_str());
    }
}

void QCustomListWidget::setIcon(QIcon &icon)
{
    button_->setIcon(icon);
}

void QCustomListWidget::showList()
{
    toggledPopup();
}
