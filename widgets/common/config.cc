#include "config.h"
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QSaveFile>
#include <QTextStream>
#include <QtDebug>

namespace QLW {

const char *Config::THEME_KEY = "theme";
Config *Config::Self = nullptr;

Config::Config(const QString &path) : _path(path) { this->load(); }

Config::~Config() { this->save(); }

Config *Config::instance(const QString &path) {
    if (Config::Self == nullptr) {
        static QMutex mutex;
        QMutexLocker locker{&mutex};

        if (Config::Self == nullptr) {
            Config::Self = new Config(path);
        }
    }
    return Config::Self;
}

void Config::load() {
    QFile file(_path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream ts(&file);
        QString content;
        try {
            while (!ts.atEnd()) {
                content.append(ts.readLine());
            }
            _cfg = Json::parse(content.toStdString());
        } catch (...) {
            qCritical() << "load config error!";
        }
        file.close();
    }
}

void Config::save() {
    QSaveFile file(_path);
    try {
        auto content = _cfg.dump(4);
        file.write(content.data());
    } catch (...) {
        qCritical() << "save config error!";
        file.cancelWriting();
    }
    file.commit();
}

Theme Config::getTheme() const {
    Theme theme = Theme::LIGHT;
    if (_cfg.contains(THEME_KEY) && _cfg[THEME_KEY].is_string()) {
        std::string t;
        _cfg[THEME_KEY].get_to(t);
        theme = (t == "dark") ? Theme::DARK : Theme::LIGHT;
    }
    return theme;
}

void Config::setTheme(Theme theme) {
    const char *t = (theme == Theme::DARK) ? "dark" : "light";
    _cfg[THEME_KEY] = t;
    emit on_ThemeChanged(theme);
}

} // namespace QLW