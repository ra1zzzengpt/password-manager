//
// Created by arch on 9/17/26.
//

#ifndef PASSWORD_MANAGER_CUSTOM_LIST_WIDGET_HPP
#define PASSWORD_MANAGER_CUSTOM_LIST_WIDGET_HPP
#include <QListWidget>
#include <QPushButton>
#include <QWidget>

class QCustomListWidget : public QWidget {
    Q_OBJECT
public:
    explicit QCustomListWidget(QWidget* parent = nullptr);

    void addOption(const std::string& text);
    void addOptions(const std::vector<std::string>& options);

    void setIcon(QIcon& icon);
public slots:
    void showList();
signals:
    void optionSelected(const QString& text);
private slots:
    void toggledPopup();
    void itemClicked(const QListWidgetItem* item);
private:
    QPushButton* button_;
    QListWidget* list_;
};


#endif //PASSWORD_MANAGER_CUSTOM_LIST_WIDGET_HPP
