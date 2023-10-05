/**
* @File icon.h
* @Date 2023-10-01
* @Description      
* @Author Ticks
* @Email ticks.cc\@gmail.com
* 
* Copyright (c) 2023 ${ORGANIZATION_NAME}. All rights reserved.
**/

#ifndef __QTFLUENTWIDGETS_ICON_H__
#define __QTFLUENTWIDGETS_ICON_H__

#include <QObject>
#include <QtXml/QDomDocument>
#include <QRect>
#include <QIcon>
#include <QIconEngine>
#include <QColor>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QStringView>
#include <QtSvg/QSvgRenderer>
#include "config.h"


namespace QtFluentWidgets
{

	QString GetIconColor(Theme theme = Theme::Auto, bool reverse = false);

	void DrawSvgIcon(QIcon icon, QPainter painter, QRect rect);

	QString WriteSvg(const QString& path, int indexes = -1,
		const QMap<QString, QString>& attributes = QMap<QString, QString>{});

	class SvgIconEngine : public QIconEngine
	{
	 public:
		SvgIconEngine(QString svg)
			: _svg(qMove(svg))
		{
		}

		void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
		QIconEngine* clone() const override;
		QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

	 private:
		QString _svg;

	}; // class SvgIconEngine

	class FluentIcon
	{
	 public:
		enum Icons
		{
			Up = 1,
			Add,
			Bus,
			Car,
			IOT,
			Pin,
			Tag,
			VPN,
			Cafe,
			Chat,
			Copy,
			Code,
			Down,
			Edit,
			Flag,
			Font,
			Game,
			Help,
			Hide,
			Home,
			Info,
			Leaf,
			Link,
			Mail,
			Menu,
			Mute,
			More,
			Move,
			Play,
			Save,
			Send,
			Sync,
			Unit,
			View,
			Wifi,
			Zoom,
			Album,
			Brush,
			Broom,
			Close,
			Cloud,
			Embed,
			Globe,
			Heart,
			Label,
			Media,
			Movie,
			Music,
			Robot,
			Pause,
			Paste,
			Photo,
			Print,
			Share,
			Tiles,
			Unpin,
			Video,
			Train,
			AddTo,
			Accept,
			Camera,
			Cancel,
			Delete,
			Folder,
			Filter,
			Market,
			Scroll,
			Layout,
			Github,
			Update,
			Remove,
			Return,
			People,
			QRCode,
			Ringer,
			Rotate,
			Search,
			Volume,
			Frigid,
			SaveAs,
			ZoomIn,
		};

		QString path(Theme theme = Theme::Auto) const;

		QIcon icon(Theme theme = Theme::Auto, QColor color = QColor{}) const;

	 private:
		Icons _icon_enum;

	}; // class FluentIconBase

} // namespace QtFluentWidgets


#endif // __QTFLUENTWIDGETS_ICON_H__
