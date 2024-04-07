/**
 * @author: Ticks
 * @email: ticks.cc@gmail.com
 */

#pragma once

#include <QMap>
#include <QObject>

class QWidget;

namespace QLW {

class StyleSheetBase;
class StyleSheetManager;
class StyleSheetManagerPrivate {
    Q_DISABLE_COPY(StyleSheetManagerPrivate);
    Q_DECLARE_PUBLIC(StyleSheetManager);

public:
    StyleSheetManagerPrivate(StyleSheetManager *q);
    ~StyleSheetManagerPrivate();

    void init();

    StyleSheetManager *const q_ptr;
    QMap<QWidget *, StyleSheetBase *> widgets;

}; // class StyleSheetManagerPrivate

} // namespace QLW