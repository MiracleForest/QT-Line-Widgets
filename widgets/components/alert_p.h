#pragma once

#include <QtGlobal>

class QPushButton;
class QPixmap;
class QLabel;

namespace QLW
{

class Alert;

class AlertPrivate
{
    Q_DISABLE_COPY(AlertPrivate);
    Q_DECLARE_PUBLIC(QLW::Alert);

public:
    AlertPrivate(QLW::Alert *q);
    ~AlertPrivate();

    void init();

    QLW::Alert *const q_ptr;

    // Logo
    QPixmap *ui_logo;
    // 标题
    QLabel *ui_label;
    // 内容
    QLabel *ui_content;
    // 关闭按钮
    QPushButton *ui_close;

}; // class AlertPrivate

} // namespace QLW