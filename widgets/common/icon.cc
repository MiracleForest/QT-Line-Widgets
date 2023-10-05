/**
* @File icon.cc
* @Date 2023-10-05
* @Description      
* @Author Ticks
* @Email ticks.cc\@gmail.com
* 
* Copyright (c) 2023 ${ORGANIZATION_NAME}. All rights reserved.
**/
#include "icon.h"
#include <QFile>

namespace QtFluentWidgets
{
	QString GetIconColor(Theme theme, bool reverse)
	{
		const char* color[2]{"black", "white"};
		int lc = 1, dc = 0;
		if (reverse) {
			lc = 0;
			dc = 1;
		}
		if (theme == Theme::Dark) {
			return color[dc];
		}else {
			return color[lc];
		}
	}
	QString WriteSvg(const QString& path, int indexes, const QMap<QString, QString>& attributes)
	{
		if (!path.endsWith(".svg")) {
			return "";
		}
		QFile file{path};
		file.open(QIODeviceBase::ReadOnly);
		QDomDocument dom{};
		dom.setContent(file.readAll());
		file.close();

		auto path_nodes = dom.elementsByTagName("path");
		if (indexes == -1) {
			indexes = path_nodes.length();
		}
		for (int i = 0; i < indexes; ++i) {
			auto elem = path_nodes.at(i).toElement();
			auto attrs = attributes.toStdMap();
			for (auto & attr : attrs) {
				elem.setAttribute(attr.first, attr.second);
			}
		}
		return dom.toString();
	}
	void DrawSvgIcon(const QByteArray& icon, QPainter* painter, QRect rect)
	{
		auto render = QSvgRenderer{icon};
		render.render(painter, QRectF(rect));
	}

	void SvgIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
	{
		DrawSvgIcon(_svg.toUtf8(), painter, rect);
	}
	QIconEngine* SvgIconEngine::clone() const
	{
		return new SvgIconEngine{this->_svg};
	}
	QPixmap SvgIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
	{
		QImage image{size, QImage::Format::Format_ARGB32};
		image.fill(Qt::GlobalColor::transparent);
		QPixmap pixmap = QPixmap::fromImage(image, Qt::NoFormatConversion);

		QPainter painter{&pixmap};
		QRect rect{0, 0, size.width(), size.height()};
		this->paint(&painter, rect, mode, state);
		return pixmap;
	}

	QString FluentIcon::path(Theme theme) const
	{
		QString s{":/qt_fluent_widgets/images/icons/"};
		auto name = magic_enum::enum_name<Icons>(_icon_enum);
		s += QString(name.data()) + "_";
		s += GetIconColor(theme);
		s += ".svg";
		return s;
	}
	QIcon FluentIcon::icon(Theme theme, QColor color) const
	{
		auto path = this->path(theme);
		if (!path.endsWith(".svg") && !color.isValid()) {
			return QIcon{path};
		}
		auto color_name = color.name();
		return QIcon{new SvgIconEngine{WriteSvg(path, -1, {{"fill", color_name}})}};
	}

} // namespace QtFluentWidgets