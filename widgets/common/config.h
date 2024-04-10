/**
 * @author: Ticks
 * @email: ticks.cc@gmail.com
 */

#pragma once

#include "3rdparty/json/nlohmann_json.h"
#include <QObject>

namespace QLW {

using Json = nlohmann::json;

// 主题
enum Theme { LIGHT, DARK, AUTO };

// 配置
class Config : public QObject {
    Q_OBJECT;
    Q_PROPERTY(Theme theme READ getTheme WRITE setTheme);

signals:
    void on_ThemeChanged(Theme theme);

public:
    ~Config();
    static Config *instance(const QString &path = "config/config.json");
    // 保存配置
    void save();
    // 获取主题
    Theme getTheme() const;
    // 设置主题
    void setTheme(Theme theme);

    Json operator()() { return _cfg; }

private:
    explicit Config(const QString &path);
    void load();

private:
    static Config *Self;

    static const char *THEME_KEY;

private:
    // 配置文件路径
    QString _path;
    // Json 配置内容
    Json _cfg;

}; // class Config

} // namespace QLW