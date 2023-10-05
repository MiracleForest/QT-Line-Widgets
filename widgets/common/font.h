/**
* @File font.h
* @Date 2023-10-01
* @Description      
* @Author Ticks
* @Email ticks.cc\@gmail.com
* 
* Copyright (c) 2023 ${ORGANIZATION_NAME}. All rights reserved.
**/

#ifndef __QTFLUENTWIDGETS_FONT_H__
#define __QTFLUENTWIDGETS_FONT_H__

#include <QWidget>
#include <QFont>

namespace QtFluentWidgets
{
	static QFont getFont(int font_size = 14, QFont::Weight weight = QFont::Normal)
	{
		auto font = QFont{};
		font.setFamilies({"Segoe UI", "Microsoft YaHei", "PingFang SC"});
		font.setPixelSize(font_size);
		font.setWeight(weight);
	}

	static void setFont(QWidget* widget,
						int font_size = 14,
						QFont::Weight weight = QFont::Normal)
	{
		widget->setFont(getFont(font_size, weight));
	}
}

#endif // __QTFLUENTWIDGETS_FONT_H__
