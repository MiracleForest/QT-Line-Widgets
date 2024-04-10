/**
 * @author: Ticks
 * @email: ticks.cc@gmail.com
 */

#pragma once

#include <QDialog>

namespace QLW {

class AlertPrivate;

class Alert : public QDialog {
    Q_OBJECT;

    Q_PROPERTY(QString style WRITE setStyle READ style);

public:
    enum Style { Info, Success, Error, Warn };

public:
    explicit Alert(QWidget *parent);
    explicit Alert(QWidget *parent, const QString &title,
                   const QString &content);
    ~Alert();

public:
    // 获取样式
    QString style() const;
    // 设置样式
    bool setStyle(Style style);

protected:
    const QScopedPointer<QLW::AlertPrivate> d_ptr;

private:
    Q_DISABLE_COPY(Alert);
    Q_DECLARE_PRIVATE(QLW::Alert);

}; // class Alert

} // namespace QLW