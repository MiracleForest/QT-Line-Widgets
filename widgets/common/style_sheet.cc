#include "style_sheet.h"

#include <QFile>
#include <QList>
#include <QMutex>
#include <QMutexLocker>

namespace QLW
{

/*------------------- StyleSheetManager -------------------*/

StyleSheetManager* StyleSheetManager::Self = nullptr;

StyleSheetManager::StyleSheetManager()
    : QObject()
{
    this->init();
}

StyleSheetManager* StyleSheetManager::instance()
{
    if (StyleSheetManager::Self == nullptr) {
        static QMutex mutex;
        QMutexLocker locker{&mutex};

        if (StyleSheetManager::Self == nullptr) {
            StyleSheetManager::Self = new StyleSheetManager();
        }
    }
    return StyleSheetManager::Self;
}

void StyleSheetManager::init() {}

void StyleSheetManager::reg(StyleSheetBase* source, QWidget* widget, bool reset)
{

    if (!_widgets.contains(widget)) {
        connect(widget, &QWidget::destroyed, this,
                [this](QObject* obj) { this->deReg((QWidget*)(obj)); });
        StyleSheetManager::Item item{
            new StyleSheetCompose({source, new CustomStyleSheet(widget)}),
            new CustomStyleSheetWatcher, new LineStyleSheetWatcher};
        widget->installEventFilter(item.custom_watcher.get());
        widget->installEventFilter(item.line_watcher.get());
        _widgets[widget] = item;
        return;
    }
    if (!reset) {
        auto s =
            dynamic_cast<StyleSheetCompose*>(_widgets[widget].source.get());
        s->add(source);
    } else {
        _widgets[widget].source.reset(
            new StyleSheetCompose({source, new CustomStyleSheet(widget)}));
    }
}

void StyleSheetManager::deReg(QWidget* widget)
{
    const auto it = _widgets.find(widget);
    if (it != _widgets.end()) {
        _widgets.erase(it);
    }
}

auto& StyleSheetManager::items() { return _widgets; }

/*------------------- StyleSheetBase -------------------*/

QString StyleSheetBase::styleContent(Theme theme)
{
    return getStyleSheetFromFile(this->stylePath(theme));
}

void StyleSheetBase::apply(QWidget* widget, Theme theme)
{
    setThemeStyleSheet(widget, this, theme);
}

/*------------------- Other StyleSheet -------------------*/

const char* CustomStyleSheet::LIGHT_QSS_KEY = "light_custom_qss";
const char* CustomStyleSheet::DARK_QSS_KEY = "dark_custom_qss";

QString CustomStyleSheet::styleContent(Theme theme)
{
    theme = Config::instance()->getTheme();
    if (theme == Theme::LIGHT) {
        return lightStyleSheet();
    }
    return darkStyleSheet();
}

StyleSheetCompose::StyleSheetCompose(
    std::initializer_list<StyleSheetBase*> list)
    : _list(list)
{
}
StyleSheetCompose::~StyleSheetCompose()
{
    for (auto it : _list) {
        if (it != nullptr) {
            delete it;
        }
    }
}
QString StyleSheetCompose::styleContent(Theme theme)
{
    QString content;
    for (auto it : _list) {
        content.append(it->styleContent(theme));
        content.append('\n');
    }
    return content;
}
void StyleSheetCompose::add(StyleSheetBase* source)
{
    if (source == this || _list.contains(source)) {
        return;
    }
    _list.push_back(source);
}
void StyleSheetCompose::remove(StyleSheetBase* source)
{
    _list.removeIf([source](StyleSheetBase* s) { return source == s; });
}

/*------------------- StyleSheetWatcher -------------------*/

const char* LineStyleSheetWatcher::LINE_PROPERTY_KEY = "line-property-key";

bool CustomStyleSheetWatcher::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::DynamicPropertyChange &&
        watched->isWidgetType()) {

        auto e = dynamic_cast<QDynamicPropertyChangeEvent*>(event);
        auto widget = qobject_cast<QWidget*>(watched);
        QString name{e->propertyName()};

        if (name == CustomStyleSheet::LIGHT_QSS_KEY ||
            name == CustomStyleSheet::DARK_QSS_KEY) {
            addThemeStyleSheet(widget, new CustomStyleSheet(widget));
        }
    }
    return QObject::eventFilter(watched, event);
}

bool LineStyleSheetWatcher::eventFilter(QObject* watched, QEvent* event)
{
    if (watched->isWidgetType() &&
        !watched->property(LINE_PROPERTY_KEY).isNull() &&
        event->type() == QEvent::Paint) {

        auto widget = qobject_cast<QWidget*>(watched);
        watched->setProperty(LINE_PROPERTY_KEY, false);

        auto& items = StyleSheetManager::instance()->items();
        auto it = items.find(widget);
        if (it != items.end()) {
            widget->setStyleSheet(getThemeStyleSheet(
                (*it).source.get(), Config::instance()->getTheme()));
        }
    }
    return QObject::eventFilter(watched, event);
}

/*------------------- Global -------------------*/

// 应用主题颜色
QString ApplyThemeColor(const QString& qss) { return qss; }

// 从文件获取样式内容
QString getStyleSheetFromFile(const QString& path)
{
    QFile file{path};
    QString content;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        content = file.readAll();
    }
    file.close();
    return content;
}

// 从 StyleSheet 获取主题样式
QString getThemeStyleSheet(StyleSheetBase* source, Theme theme)
{
    return ApplyThemeColor(source->styleContent(theme));
}
QString getThemeStyleSheet(const QString& path, Theme theme)
{
    QScopedPointer<FileStyleSheet> source(new FileStyleSheet(path));
    return ApplyThemeColor(source->styleContent(theme));
}

// 为组件设置主题样式
void setThemeStyleSheet(QWidget* widget, StyleSheetBase* source, Theme theme,
                        bool reg)
{
    if (reg) {
        StyleSheetManager::instance()->reg(source, widget);
    }
    widget->setStyleSheet(getThemeStyleSheet(source, theme));
}
// 设置自定义样式
void setCustomStyleSheet(QWidget* widget, const QString& light_qss,
                         const QString& dark_qss)
{
    CustomStyleSheet(widget).setCustomStyleSheet(light_qss, dark_qss);
}

// 添加样式
void addThemeStyleSheet(QWidget* widget, StyleSheetBase* source, Theme theme,
                        bool reg)
{
    QString qss;
    if (reg) {
        StyleSheetManager::instance()->reg(source, widget, false);
        qss = getThemeStyleSheet(source, theme);
    } else {
        qss = widget->styleSheet() + '\n' + getThemeStyleSheet(source, theme);
    }

    if (qss != widget->styleSheet()) {
        widget->setStyleSheet(qss);
    }
}
void addThemeStyleSheet(QWidget* widget, const QString& path, Theme theme,
                        bool reg)
{
    QString qss;
    if (reg) {
        StyleSheetManager::instance()->reg(new FileStyleSheet(path), widget,
                                           false);
        qss = getThemeStyleSheet(path, theme);
    } else {
        qss = widget->styleSheet() + '\n' + getThemeStyleSheet(path, theme);
    }

    if (qss != widget->styleSheet()) {
        widget->setStyleSheet(qss);
    }
}

// 更新样式
void updateStyleSheet(bool lazy)
{
    QList<QWidget*> removes;
    const auto& items = StyleSheetManager::instance()->items();
    for (auto& it : items.toStdMap()) {
        auto widget = it.first;
        auto source = it.second.source.get();

        try {
            if (!lazy && !widget->visibleRegion().isNull()) {
                widget->setStyleSheet(
                    getThemeStyleSheet(source, Config::instance()->getTheme()));
            } else {
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

// 设置主题
void setTheme(Theme theme)
{
    Config::instance()->setTheme(theme);
    updateStyleSheet();
}
// 切换主题
void toggleTheme()
{
    auto t = Config::instance()->getTheme() == Theme::DARK ? Theme::LIGHT
                                                           : Theme::DARK;
    setTheme(t);
}

} // namespace QLW