/**
 * @author: Ticks
 * @email: ticks.cc@gmail.com
 */

#pragma once

#include <QDialog>

namespace QLW
{

class AlertPrivate;

class Alert : public QDialog
{
    Q_OBJECT;

    Q_PROPERTY(Alert::StyleType styleType WRITE setStyleType READ styleType);
    Q_PROPERTY(QString title WRITE setTitle READ title);
    Q_PROPERTY(QString content WRITE setContent READ content);

public:
    enum StyleType
    {
        Info,
        Success,
        Error,
    };

public:
    explicit Alert(QWidget* parent = nullptr);
    explicit Alert(const QString& title, const QString& content,
                   QWidget* parent = nullptr);
    ~Alert();

public:
    // 获取样式
    [[nodiscard]] auto styleType() const -> StyleType;
    // 设置样式
    void setStyleType(StyleType style);

    [[nodiscard]] auto title() const -> QString;
    void setTitle(const QString& title);

    [[nodiscard]] auto content() const -> QString;
    void setContent(const QString& content);

signals:
    void on_titleChanged(const QString&);
    void on_contentChanged(const QString&);
    void on_closed();

protected:
    void paintEvent(QPaintEvent* event) override;

protected:
    const QScopedPointer<QLW::AlertPrivate> d_ptr;

private:
    Q_DISABLE_COPY(Alert);
    Q_DECLARE_PRIVATE(QLW::Alert);

}; // class Alert

} // namespace QLW

Q_ENUMS(QLW::Alert::StyleType);
Q_DECLARE_METATYPE(QLW::Alert::StyleType);