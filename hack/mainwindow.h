#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    int ProcessHack(const char* new_password, const char* filename);
    const char* GetPassword(QLineEdit* input);

    ~MainWindow();

private slots:
    void HandleInput(QLineEdit* input, QLabel* error_label);
    void OpenFile();

private:
    Ui::MainWindow *ui;
    QLabel* background;
    QPushButton* button;
    QString file_name;

};

char* ToCStr(QString str);

#endif // MAINWINDOW_H
