#pragma once

#include <QString>
#include <QtGlobal>

class QPushButton;
class QLabel;

namespace QLW
{

class Alert;

class AlertPrivate
{
    Q_DISABLE_COPY(AlertPrivate);
    Q_DECLARE_PUBLIC(QLW::Alert);

public:
    AlertPrivate(QLW::Alert* q);
    ~AlertPrivate();

    void init();

public:
    QLW::Alert* const q_ptr;

    // Logo
    QLabel* ui_logo;
    // 标题
    QLabel* ui_title;
    // 内容
    QLabel* ui_content;
    // 关闭按钮
    QPushButton* ui_close;

    // 标题
    QString title;
    // 内容
    QString content;
    // 样式
    QString style;

}; // class AlertPrivate

} // namespace QLW