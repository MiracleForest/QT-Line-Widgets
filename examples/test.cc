#include "common/style_sheet.h"
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>

class MyStyleSheet : public QLW::StyleSheetBase {
public:
    ~MyStyleSheet() = default;

    QString stylePath(QLW::Theme theme = QLW::Theme::LIGHT) override {
        return "";
    }
    QString styleContent(QLW::Theme theme = QLW::Theme::LIGHT) override {
        if (theme == QLW::LIGHT) {
            return "QPushButton { background-color: #f5f5f5; color: #232730; }";
        } else {
            return "QPushButton { background-color: #232730; color: #f5f5f5; }";
        }
    }
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QMainWindow window;
    window.setMinimumSize(640, 380);

    auto button = new QPushButton(&window);
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

    window.show();
    return app.exec();
}