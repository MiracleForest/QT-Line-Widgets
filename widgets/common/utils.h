/**
 * @author: Ticks
 * @email: ticks.cc@gmail.com
 */

#pragma once

#include <QIcon>

#include "config.h"

namespace QLW
{
// 从文件加载Svg图片
QIcon LoadSvgIcon(const QString& path);
QIcon LoadSvgIcon(const QString& path, const QColor& color);
QIcon LoadSvgIcon(const QString& path, const QSize& size);
QIcon LoadSvgIcon(const QString& path, const QSize& size, const QColor& color);

// 获取主题资源文件
QString GetThemeIconResFile(const QString& name, Theme theme);

} // namespace QLW
