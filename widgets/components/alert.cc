#include "alert.h"
#include "alert_p.h"

#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>

#include "3rdparty/magic_enum/magic_enum.hpp"

namespace QLW {

AlertPrivate::AlertPrivate(Alert *q) : q_ptr(q) {}

AlertPrivate::~AlertPrivate() {}

void AlertPrivate::init() {
    Q_Q(Alert);

    q->setAttribute(Qt::WA_TranslucentBackground);
    q->setWindowFlags(Qt::FramelessWindowHint);
}

/*-------------------------------------*/

Alert::Alert(QWidget *parent) : QDialog(parent), d_ptr(new AlertPrivate(this)) {
    Q_D(Alert);
    d->init();
}

Alert::Alert(QWidget *parent, const QString &title, const QString &content)
    : QDialog(parent), d_ptr(new AlertPrivate(this)) {
    Q_D(Alert);
    d->init();
}

Alert::~Alert() {}

QString Alert::style() const {
    auto s = property("style");
    return !s.isNull() ? s.toString() : "Info";
}

bool Alert::setStyle(Style style) {
    auto name = magic_enum::enum_name(style);
    return setProperty("style", QString::fromStdString(std::string{name}));
}

} // namespace QLW