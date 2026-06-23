#include <QApplication>
#include <fstream>
#include <sstream>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QString fileContent;
    QString filePath;
    if (argc >= 2) {
        filePath = argv[1];
        std::ifstream file(argv[1]);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            fileContent = QString::fromStdString(content);
        }
    }

    MainWindow window(nullptr, fileContent, filePath);
    window.show();
    return app.exec();
}
