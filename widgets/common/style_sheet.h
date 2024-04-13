/**
 * @author: Ticks
 * @email: ticks.cc@gmail.com
 */

#pragma once

#include <QEvent>
#include <QObject>
#include <QVariant>
#include <QWidget>
#include <format>

#include "3rdparty/magic_enum/magic_enum.hpp"
#include "config.h"

namespace QLW
{

class StyleSheetBase;
class CustomStyleSheetWatcher;
class LineStyleSheetWatcher;

// 样式管理
class StyleSheetManager : public QObject
{
    Q_OBJECT;

public:
    class Item
    {
    public:
        Item() = default;
        explicit Item(StyleSheetBase *s, CustomStyleSheetWatcher *c_watcher,
                      LineStyleSheetWatcher *l_watcher)
            : source(s)
            , custom_watcher(c_watcher)
            , line_watcher(l_watcher)
        {
        }
        ~Item() = default;

    public:
        QSharedPointer<StyleSheetBase> source;
        QSharedPointer<CustomStyleSheetWatcher> custom_watcher;
        QSharedPointer<LineStyleSheetWatcher> line_watcher;
    };

public:
    ~StyleSheetManager() = default;
    static StyleSheetManager *instance();
    void reg(StyleSheetBase *source, QWidget *widget, bool reset = true);
    void deReg(QWidget *widget);
    auto &items();

private:
    explicit StyleSheetManager();
    void init();

protected:
    static StyleSheetManager *Self;
    QMap<QWidget *, Item> _widgets;

}; // StyleSheetManager

// 事件过滤器
class CustomStyleSheetWatcher : public QObject
{
    Q_OBJECT;

public:
    bool eventFilter(QObject *watched, QEvent *event) override;
};
class LineStyleSheetWatcher : public QObject
{
    Q_OBJECT;

public:
    static const char *LINE_PROPERTY_KEY;
    bool eventFilter(QObject *watched, QEvent *event) override;
};

// 基础样式
class StyleSheetBase
{
public:
    StyleSheetBase() = default;
    virtual ~StyleSheetBase() = default;
    // 样式路径，必须实现
    virtual QString stylePath(Theme theme = Theme::LIGHT) = 0;
    // 样式内容
    virtual QString styleContent(Theme theme = Theme::LIGHT);
    // 应用样式
    void apply(QWidget *widget, Theme theme = Theme::LIGHT);

}; // class StyleSheetBase

class StyleSheetCompose : public StyleSheetBase
{
public:
    StyleSheetCompose(std::initializer_list<StyleSheetBase *> list);
    ~StyleSheetCompose() override;
    QString stylePath(Theme theme = Theme::LIGHT) override { return ""; }
    QString styleContent(Theme theme = Theme::LIGHT) override;

    void add(StyleSheetBase *source);
    void remove(StyleSheetBase *source);

private:
    QList<StyleSheetBase *> _list;
};

// 内置样式
enum LineStyleSheetEnum
{
    Alert,
}; // enum LineStyleSheetEnum

template <LineStyleSheetEnum E> class LineStyleSheet : public StyleSheetBase
{
public:
    ~LineStyleSheet() override = default;

    QString stylePath(Theme theme = Theme::LIGHT) override
    {
        theme = Config::instance()->getTheme();
        const char *t = (theme == Theme::LIGHT) ? "light" : "dark";
        auto path = magic_enum::enum_name(E);
        return QString::fromStdString(
            std::format(":/qlw/qss/{}/{}.qss", t, path));
    }
};

// 基于文件的样式
class FileStyleSheet : public StyleSheetBase
{
public:
    FileStyleSheet(const QString &path)
        : _path(path)
    {
    }
    ~FileStyleSheet() override = default;

    QString stylePath(Theme theme = Theme::LIGHT) override { return _path; }

private:
    QString _path;
};

// 自定义样式
class CustomStyleSheet : public StyleSheetBase
{
public:
    CustomStyleSheet(QWidget *widget)
        : _widget(widget)
    {
    }
    ~CustomStyleSheet() override = default;

    QString stylePath(Theme theme = Theme::LIGHT) override { return ""; }

    QString styleContent(Theme theme = Theme::LIGHT) override;

    void setCustomStyleSheet(const QString &light_qss, const QString &dark_qss)
    {
        setLightStyleSheet(light_qss);
        setDarkStyleSheet(dark_qss);
    }
    void setLightStyleSheet(const QString &qss)
    {
        _widget->setProperty(LIGHT_QSS_KEY, qss);
    }
    void setDarkStyleSheet(const QString &qss)
    {
        _widget->setProperty(DARK_QSS_KEY, qss);
    }
    QString lightStyleSheet() const
    {
        auto s = _widget->property(LIGHT_QSS_KEY);
        return !s.isNull() ? s.toString() : "";
    }
    QString darkStyleSheet() const
    {
        auto s = _widget->property(DARK_QSS_KEY);
        return !s.isNull() ? s.toString() : "";
    }

public:
    static const char *LIGHT_QSS_KEY;
    static const char *DARK_QSS_KEY;

private:
    QWidget *_widget;
};

// 从文件获取样式内容
QString getStyleSheetFromFile(const QString &path);
// 从 StyleSheet 获取主题样式内容
QString getThemeStyleSheet(StyleSheetBase *source, Theme theme = Theme::LIGHT);
// 从文件路径获取主题样式内容
QString getThemeStyleSheet(const QString &path, Theme theme = Theme::LIGHT);
// 为组件设置主题样式
void setThemeStyleSheet(QWidget *widget, StyleSheetBase *source,
                        Theme theme = Theme::LIGHT, bool reg = true);
// 为组件设置自定义样式
void setCustomStyleSheet(QWidget *widget, const QString &light_qss,
                         const QString &dark_qss);
// 为组件添加主题样式
void addThemeStyleSheet(QWidget *widget, StyleSheetBase *source,
                        Theme theme = Theme::LIGHT, bool reg = true);
void addThemeStyleSheet(QWidget *widget, const QString &path,
                        Theme theme = Theme::LIGHT, bool reg = true);

// 更新样式
void updateStyleSheet(bool lazy = false);

// 设置主题
void setTheme(Theme theme);
// 切换主题
void toggleTheme();

} // namespace QLW