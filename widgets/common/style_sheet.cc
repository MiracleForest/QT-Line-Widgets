#include "style_sheet.h"
#include "style_sheet_p.h"

#include <QFile>
#include <QList>
#include <QMutex>
#include <QMutexLocker>

namespace QLW {

// 应用主题颜色
QString ApplyThemeColor(const QString &qss) { return qss; }

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

    auto it = d->widgets.find(widget);

    if (it == d->widgets.end()) {
        connect(widget, &QWidget::destroyed, this,
                [this](QObject *obj) { this->deReg((QWidget *)(obj)); });
        // !!! 存在内存泄露
        widget->installEventFilter(new CustomStyleSheetWatcher);
        widget->installEventFilter(new LineStyleSheetWatcher);
    } else {
        delete *it;
    }
    *it = source;
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

auto &StyleSheetManager::items() {
    Q_D(StyleSheetManager);
    return d->widgets;
}

QString StyleSheetBase::styleContent(Theme theme) {
    return getStyleSheetFromFile(this->stylePath(theme));
}

void StyleSheetBase::apply(QWidget *widget, Theme theme) {
    setStyleSheet(widget, this, theme);
}

const char *CustomStyleSheet::LIGHT_QSS_KEY = "light_custom_qss";
const char *CustomStyleSheet::DARK_QSS_KEY = "dark_custom_qss";
const char *LineStyleSheetWatcher::LINE_PROPERTY_KEY = "line-property-key";

bool CustomStyleSheetWatcher::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::DynamicPropertyChange &&
        watched->isWidgetType()) {

        auto e = dynamic_cast<QDynamicPropertyChangeEvent *>(event);
        auto widget = qobject_cast<QWidget *>(watched);

        QString name{e->propertyName()};
        if (name == CustomStyleSheet::LIGHT_QSS_KEY ||
            name == CustomStyleSheet::DARK_QSS_KEY) {
            addStyleSheet(widget, new CustomStyleSheet(widget));
        }
    }
    return QObject::eventFilter(watched, event);
}

bool LineStyleSheetWatcher::eventFilter(QObject *watched, QEvent *event) {
    if (watched->isWidgetType() &&
        !watched->property(LINE_PROPERTY_KEY).isNull() &&
        event->type() != QEvent::Paint) {

        auto widget = qobject_cast<QWidget *>(watched);
        watched->setProperty(LINE_PROPERTY_KEY, false);

        auto &items = StyleSheetManager::instance()->items();
        auto it = items.find(widget);
        if (it != items.end()) {
            widget->setStyleSheet(
                getStyleSheet(*it, Config::instance()->getTheme()));
        }
    }
    return QObject::eventFilter(watched, event);
}

// 从文件获取样式
QString getStyleSheetFromFile(const QString &path) {
    QFile file{path};
    QString content;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        content = file.readAll();
    }
    file.close();
    return content;
}
QString getStyleSheet(StyleSheetBase *source, Theme theme) {
    return ApplyThemeColor(source->styleContent(theme));
}
QString getStyleSheet(const QString &path, Theme theme) {
    QScopedPointer<FileStyleSheet> source(new FileStyleSheet(path));
    return ApplyThemeColor(source->styleContent(theme));
}

// 设置样式
void setStyleSheet(QWidget *widget, StyleSheetBase *source, Theme theme) {
    StyleSheetManager::instance()->reg(source, widget);
    widget->setStyleSheet(getStyleSheet(source, theme));
}
// 设置自定义样式
void setCustomStyleSheet(QWidget *widget, const QString &light_qss,
                         const QString &dark_qss) {
    CustomStyleSheet(widget).setCustomStyleSheet(light_qss, dark_qss);
}

// 添加样式
void addStyleSheet(QWidget *widget, StyleSheetBase *source, Theme theme,
                   bool reg) {
    QString qss;
    if (reg) {
        StyleSheetManager::instance()->reg(source, widget);
        qss = getStyleSheet(source, theme);
    } else {
        qss = widget->styleSheet() + '\n' + getStyleSheet(source, theme);
    }

    if (qss != widget->styleSheet()) {
        widget->setStyleSheet(qss);
    }
}
void addStyleSheet(QWidget *widget, const QString &path, Theme theme,
                   bool reg) {
    QString qss;
    if (reg) {
        StyleSheetManager::instance()->reg(source, widget);
        qss = getStyleSheet(path, theme);
    } else {
        qss = widget->styleSheet() + '\n' + getStyleSheet(path, theme);
    }

    if (qss != widget->styleSheet()) {
        widget->setStyleSheet(qss);
    }
}

// 更新样式
void updateStyleSheet(bool lazy) {
    QList<QWidget *> removes;
    const auto &items = StyleSheetManager::instance()->items();
    for (auto &it : items.toStdMap()) {
        auto widget = it.first;
        auto source = it.second;

        try {
            if (!lazy && !widget->visibleRegion().isNull()) {
                setStyleSheet(widget, source, Config::instance()->getTheme());
            } else {
                StyleSheetManager::instance()->reg(source, widget);
                widget->setProperty(LineStyleSheetWatcher::LINE_PROPERTY_KEY,
                                    true);
            }
        } catch (...) {
            removes.append(widget);
        }
    }
    auto manager = StyleSheetManager::instance();
    for (auto it : removes) {
        manager->deReg(it);
    }
}

} // namespace QLW