#include "alert.h"
#include "alert_p.h"
#include "common/style_sheet.h"
#include "common/utils.h"

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>

namespace QLW
{

AlertPrivate::AlertPrivate(Alert* q)
    : q_ptr(q)
    , ui_logo(new QLabel())
    , ui_title(new QLabel())
    , ui_content(new QLabel())
    , ui_close(new QPushButton())
{
}

AlertPrivate::~AlertPrivate() {}

void AlertPrivate::init()
{
    Q_Q(Alert);

    q->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    q->setAttribute(Qt::WA_Hover);
    q->setAttribute(Qt::WA_TranslucentBackground);

    // Init UI
    auto theme = Config::instance()->getTheme();

    QString logo_path;
    auto style = q->styleType();
    if (style == Alert::Success) {
        logo_path = GetThemeIconResFile("success.svg", theme);
    } else if (style == Alert::Error) {
        logo_path = GetThemeIconResFile("error.svg", theme);
    } else {
        logo_path = GetThemeIconResFile("info.svg", theme);
    }

    ui_logo->setObjectName("alert_logo");
    ui_logo->setPixmap(QIcon(logo_path).pixmap({32, 32}));

    ui_title->setObjectName("alert_title");
    auto font_title = ui_title->font();
    font_title.setPixelSize(18);
    ui_title->setFont(font_title);

    ui_content->setObjectName("alert_content");
    ui_close->setObjectName("alert_close");
    ui_close->setCursor(Qt::PointingHandCursor);
    ui_close->setIcon(QIcon(GetThemeIconResFile("close.svg", theme)));
    ui_close->setIconSize({16, 16});

    auto ui_layout = new QGridLayout();
    ui_layout->addWidget(ui_logo, 0, 0);
    ui_layout->addWidget(ui_title, 0, 1, 1, 3);
    ui_layout->addWidget(ui_content, 1, 1, 1, 3);
    ui_layout->addWidget(ui_close, 0, 4);

    // 间距
    ui_layout->setHorizontalSpacing(20);
    ui_layout->setVerticalSpacing(10);
    ui_layout->setContentsMargins(10, 10, 10, 10);

    auto frame = new QFrame();
    frame->setObjectName("alert_box");
    auto dialog_layout = new QVBoxLayout();
    dialog_layout->addWidget(frame);
    frame->setLayout(ui_layout);
    q->setLayout(dialog_layout);

    QObject::connect(q, &Alert::on_titleChanged, [this](const QString& title) {
        this->ui_title->setText(title);
    });
    QObject::connect(
        q, &Alert::on_contentChanged,
        [this](const QString& content) { this->ui_content->setText(content); });
    QObject::connect(ui_close, &QPushButton::clicked, [q] { q->close(); });

    LineStyleSheet<LineStyleSheetEnum::alert>::create()->apply(q);
}

/*-------------------------------------*/

Alert::Alert(QWidget* parent)
    : QDialog(parent)
    , d_ptr(new AlertPrivate(this))
{
    Q_D(Alert);
    d->init();
}

Alert::Alert(const QString& title, const QString& content, QWidget* parent)
    : QDialog(parent)
    , d_ptr(new AlertPrivate(this))
{
    Q_D(Alert);
    d->init();
    setTitle(title);
    setContent(content);
}

Alert::~Alert() {}

void Alert::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);

    QPainter p{this};
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p);

    QDialog::paintEvent(event);
}

auto Alert::styleType() const -> Alert::StyleType
{
    Q_D(const Alert);
    return magic_enum::enum_cast<StyleType>(d->style.toStdString())
        .value_or(StyleType::Info);
}

void Alert::setStyleType(StyleType style)
{
    Q_D(Alert);
    auto s = magic_enum::enum_name(style);
    d->style = QString::fromStdString(std::string{s.data(), s.size()});
}

auto Alert::title() const -> QString
{
    Q_D(const Alert);
    return d->title;
}
void Alert::setTitle(const QString& title)
{
    Q_D(Alert);
    d->title = title;
    emit on_titleChanged(d->title);
}

QString Alert::content() const
{
    Q_D(const Alert);
    return d->content;
}
void Alert::setContent(const QString& content)
{
    Q_D(Alert);
    d->content = content;
    emit on_contentChanged(d->content);
}

} // namespace QLW