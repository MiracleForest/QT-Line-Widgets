#include "style_sheet.h"
#include "style_sheet_p.h"

#include <QMutex>
#include <QMutexLocker>

namespace QLW {

StyleSheetManagerPrivate::StyleSheetManagerPrivate(StyleSheetManager *q)
    : q_ptr(q) {}

StyleSheetManagerPrivate::~StyleSheetManagerPrivate() {}

void StyleSheetManagerPrivate::init() {}

StyleSheetManager *StyleSheetManager::Self = nullptr;

StyleSheetManager::StyleSheetManager()
    : d_ptr(new StyleSheetManagerPrivate(this)) {
    d_func()->init();
}

StyleSheetManager *StyleSheetManager::instance() {
    if (StyleSheetManager::Self == nullptr) {
        static QMutex mutex;
        QMutexLocker locker{&mutex};

        if (StyleSheetManager::Self == nullptr) {
            StyleSheetManager::Self = new StyleSheetManager();
        }
    }
    return StyleSheetManager::Self;
}

void StyleSheetManager::reg(StyleSheetBase *source, QWidget *widget) {
    Q_D(StyleSheetManager);

    if (!d->widgets.contains(widget)) {
        connect(widget, &QWidget::destroyed, this,
                [this](QObject *obj) { this->deReg((QWidget *)(obj)); });
        // todo 事件过滤器
        widget->installEventFilter(new CustomStyleSheetWatcher);
    }
    d->widgets[widget] = source;
}

void StyleSheetManager::deReg(QWidget *widget) {
    Q_D(StyleSheetManager);

    const auto it = d->widgets.find(widget);
    if (it != d->widgets.end()) {
        // 释放资源
        if (*it != nullptr) {
            delete *it;
        }
        d->widgets.erase(it);
    }
}

QString StyleSheetBase::styleContent(Theme theme) {
    return getStyleSheetFromFile(this->stylePath(theme));
}

void StyleSheetBase::apply(QWidget *widget, Theme theme) {
    setStyleSheet(widget, this, theme);
}

const char *CustomStyleSheet::LIGHT_QSS_KEY = "light_custom_qss";
const char *CustomStyleSheet::DARK_QSS_KEY = "dark_custom_qss";

} // namespace QLW