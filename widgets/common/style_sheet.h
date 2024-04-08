/**
 * @author: Ticks
 * @email: ticks.cc@gmail.com
 */

#pragma once

#include "config.h"
#include <QEvent>
#include <QObject>
#include <QVariant>
#include <QtWidgets/QWidget>
#include <format>

namespace QLW {

class StyleSheetBase;
class StyleSheetManagerPrivate;
// 样式管理
class StyleSheetManager : public QObject {
    Q_OBJECT;

public:
    ~StyleSheetManager() = default;
    static StyleSheetManager *instance();
    void reg(StyleSheetBase *source, QWidget *widget);
    void deReg(QWidget *widget);
    auto &items();

private:
    explicit StyleSheetManager();

private:
    static StyleSheetManager *Self;

private:
    const QScopedPointer<StyleSheetManagerPrivate> d_ptr;
    Q_DECLARE_PRIVATE(StyleSheetManager);

}; // StyleSheetManager

// 基础样式
class StyleSheetBase {
public:
    StyleSheetBase() = default;
    virtual ~StyleSheetBase() = default;
    // 样式路径
    virtual QString stylePath(Theme theme = Theme::LIGHT) = 0;
    // 样式内容
    virtual QString styleContent(Theme theme = Theme::LIGHT);
    // 应用
    void apply(QWidget *widget, Theme theme = Theme::LIGHT);

}; // class StyleSheetBase

enum LineStyleSheetEnum {
    BUTTON,
};

template <LineStyleSheetEnum E> class LineStyleSheet : public StyleSheetBase {
public:
    ~LineStyleSheet() override = default;

    constexpr const char *Str() const {
        if constexpr (E == BUTTON) {
            return "button";
        }
    }

    QString stylePath(Theme theme = Theme::LIGHT) override {
        theme = Config::instance()->getTheme();
        const char *t = (theme == Theme::LIGHT) ? "light" : "dark";
        const char *path = Str();
        return QString::fromStdString(
            std::format(":/qlw/qss/{}/{}.qss", t, path));
    }
};

class FileStyleSheet : public StyleSheetBase {
public:
    FileStyleSheet(const QString &path) : _path(path) {}
    ~FileStyleSheet() override = default;

    QString stylePath(Theme theme = Theme::LIGHT) override { return _path; }

private:
    QString _path;
};

// 自定义样式
class CustomStyleSheet : public StyleSheetBase {
public:
    CustomStyleSheet(QWidget *widget) : _widget(widget) {}
    ~CustomStyleSheet() override = default;

    QString stylePath(Theme theme = Theme::LIGHT) override { return ""; }

    QString styleContent(Theme theme = Theme::LIGHT) override {
        theme = Config::instance()->getTheme();
        if (theme == Theme::LIGHT) {
            return lightStyleSheet();
        }
        return darkStyleSheet();
    }

    void setCustomStyleSheet(const QString &light_qss,
                             const QString &dark_qss) {
        setLightStyleSheet(light_qss);
        setDarkStyleSheet(dark_qss);
    }
    void setLightStyleSheet(const QString &qss) {
        _widget->setProperty(LIGHT_QSS_KEY, qss);
    }
    void setDarkStyleSheet(const QString &qss) {
        _widget->setProperty(DARK_QSS_KEY, qss);
    }
    QString lightStyleSheet() const {
        auto s = _widget->property(LIGHT_QSS_KEY);
        return !s.isNull() ? s.toString() : "";
    }
    QString darkStyleSheet() const {
        auto s = _widget->property(DARK_QSS_KEY);
        return !s.isNull() ? s.toString() : "";
    }

public:
    static const char *LIGHT_QSS_KEY;
    static const char *DARK_QSS_KEY;

private:
    QWidget *_widget;
};

// 事件过滤器
class CustomStyleSheetWatcher : public QObject {
    Q_OBJECT;

public:
    bool eventFilter(QObject *watched, QEvent *event) override;
};
class LineStyleSheetWatcher : public QObject {
    Q_OBJECT;

public:
    static const char *LINE_PROPERTY_KEY;
    bool eventFilter(QObject *watched, QEvent *event) override;
};

// 从文件获取样式
QString getStyleSheetFromFile(const QString &path);
QString getStyleSheet(StyleSheetBase *source, Theme theme = Theme::LIGHT);
QString getStyleSheet(const QString &path, Theme theme = Theme::LIGHT);
// 设置样式
void setStyleSheet(QWidget *widget, StyleSheetBase *source,
                   Theme theme = Theme::LIGHT);
// 设置自定义样式
void setCustomStyleSheet(QWidget *widget, const QString &light_qss,
                         const QString &dark_qss);
// 添加样式
void addStyleSheet(QWidget *widget, StyleSheetBase *source,
                   Theme theme = Theme::LIGHT, bool reg = true);
void addStyleSheet(QWidget *widget, const QString &path,
                   Theme theme = Theme::LIGHT, bool reg = true);

// 更新样式
void updateStyleSheet(bool lazy = false);

} // namespace QLW