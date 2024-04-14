#include "utils.h"

#include <QPainter>
#include <QSvgRenderer>

namespace QLW
{

QIcon LoadSvgIcon(const QString& path)
{
    QSvgRenderer render;
    render.load(path);

    QPixmap pixmap(render.defaultSize());
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing);
    render.render(&painter);

    return {pixmap};
}
QIcon LoadSvgIcon(const QString& path, const QColor& color)
{
    QSvgRenderer render;
    render.load(path);

    QPixmap pixmap(render.defaultSize());
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setBrush(QBrush(color));
    painter.setRenderHints(QPainter::Antialiasing);
    render.render(&painter);

    return {pixmap};
}
QIcon LoadSvgIcon(const QString& path, const QSize& size)
{
    QSvgRenderer render;
    render.load(path);

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing);
    render.render(&painter);

    return {pixmap};
}
QIcon LoadSvgIcon(const QString& path, const QSize& size, const QColor& color)
{
    QSvgRenderer render;
    render.load(path);

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setBrush(QBrush(color));
    painter.setRenderHints(QPainter::Antialiasing);
    render.render(&painter);

    return {pixmap};
}

QString GetThemeIconResFile(const QString& name, Theme theme)
{
    if (theme == DARK) {
        return ":/qlw/icons/dark/" + name;
    }
    return ":/qlw/icons/light/" + name;
}

} // namespace QLW