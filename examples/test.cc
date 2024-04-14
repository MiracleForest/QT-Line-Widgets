#include "common/logger.h"
#include "common/style_sheet.h"
#include "components/alert.h"

#include <QApplication>
#include <QBoxLayout>
#include <QGroupBox>
#include <QMainWindow>
#include <QPushButton>

class MyStyleSheet : public QLW::StyleSheetBase
{
public:
    ~MyStyleSheet() = default;

    QString stylePath(QLW::Theme theme = QLW::Theme::LIGHT) override
    {
        return "";
    }
    QString styleContent(QLW::Theme theme = QLW::Theme::LIGHT) override
    {
        if (theme == QLW::LIGHT) {
            return "QPushButton { background-color: #f5f5f5; color: #232730; }";
        } else {
            return "QPushButton { background-color: #232730; color: #f5f5f5; }";
        }
    }
};

int main(int argc, char** argv)
{
    QLW::SetGlobalLogLevel(QLW::LogLevel::kALL);
    QApplication app(argc, argv);
    QMainWindow window;
    window.setMinimumSize(640, 380);

    auto box = new QGroupBox(&window);
    auto layout = new QVBoxLayout();
    auto button = new QPushButton();
    button->setText(QLW::Config::instance()->getTheme() == QLW::Theme::DARK
                        ? "dark"
                        : "light");
    QObject::connect(QLW::Config::instance(), &QLW::Config::on_ThemeChanged,
                     [button](QLW::Theme theme) {
                         button->setText(theme == QLW::Theme::DARK ? "dark"
                                                                   : "light");
                     });
    QObject::connect(button, &QPushButton::clicked, [] { QLW::toggleTheme(); });
    (new MyStyleSheet())->apply(button, QLW::Theme::LIGHT);

    auto btn1 = new QPushButton("点击");
    QObject::connect(btn1, &QPushButton::clicked, [] {
        QLW::Alert alert;
        alert.setTitle("Changes saved");
        alert.setContent("Your product changes have been saved.");
        alert.exec();
        // QDialog dialog;
        // dialog.exec();
    });

    layout->addWidget(button);
    layout->addWidget(btn1);
    box->setTitle("test");
    box->setLayout(layout);
    window.setCentralWidget(box);

    window.show();

    return app.exec();
}