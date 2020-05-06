#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMovie>
#include <QProgressBar>
#include <QLineEdit>
#include <QDebug>
#include <string.h>
#include <hack.h>

const int W_HEIGHT = 500;
const int W_WIDTH = 650;
const int BTN_WIDTH = 150;
const int BTN_HEIGHT = 70;
const int P_BAR_WIDTH = 450;
const int P_BAR_HEIGHT = 25;
const int LABEL_WIDTH = 200;
const int LABEL_HEIGHT = 40;
const int INPUT_WIDTH = 200;
const int INPUT_HEIGHT= 20;
const int PADDING = 100;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(QSize(W_WIDTH, W_HEIGHT));
    statusBar()->hide();
    setWindowTitle("HACK");

    QMovie* matrix = new QMovie(":/resources/img/matrix.gif");
    background = new QLabel(this);

    background->setScaledContents(true);
    setCentralWidget(background);
    background->setMovie(matrix);
    matrix->start();


    QMediaPlaylist* playlist = new QMediaPlaylist();
    playlist->addMedia(QUrl("qrc:/resources/sounds/music.mp3"));
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    QMediaPlayer* music = new QMediaPlayer();
    music->setPlaylist(playlist);
    music->setVolume(15);
    music->play();

    QLabel* inp_label = new QLabel("Enter new password:", background);
    inp_label->setStyleSheet("QLabel { color: white; font-size: 10pt; }");
    inp_label->setGeometry(W_WIDTH - BTN_WIDTH - INPUT_WIDTH - PADDING * 3/2,
                             (W_HEIGHT - BTN_HEIGHT - 2*INPUT_HEIGHT)/2, INPUT_WIDTH, INPUT_HEIGHT);


    QLineEdit* input = new QLineEdit(background);
    input->setGeometry(W_WIDTH - BTN_WIDTH - INPUT_WIDTH - PADDING * 3/2,
                       (W_HEIGHT  - BTN_HEIGHT + 1 * INPUT_HEIGHT)/2, INPUT_WIDTH, INPUT_HEIGHT);

    QLabel* error_label = new QLabel(background);
    error_label->setStyleSheet("QLabel { color: red; font-size: 10pt; }");
    error_label->setGeometry(W_WIDTH - BTN_WIDTH - INPUT_WIDTH - PADDING * 3/2,
                             (W_HEIGHT - BTN_HEIGHT + 8*INPUT_HEIGHT)/2, 2*INPUT_WIDTH, INPUT_HEIGHT);
    error_label->hide();

    QPushButton* file_button = new QPushButton("Select file", background);
    connect(file_button, &QPushButton::clicked, [this](){OpenFile();});
    file_button->setStyleSheet("border: none;"
                      "background: #404040;"
                      "color: #ffffff;"
                      "padding: 10px;"
                      "border-radius: 6px;");
    file_button->setGeometry(W_WIDTH - BTN_WIDTH - INPUT_WIDTH - PADDING * 3/2,
                             (W_HEIGHT + BTN_HEIGHT - 2*file_button->height())/2, file_button->width(), file_button->height());


    button = new QPushButton("HACK", background);
    button->setStyleSheet("border: none;"
                          "font-size: 20pt;"
                          "background: #404040;"
                          "color: #ffffff;"
                          "padding: 20px;"
                          "border-radius: 6px;");



    button->setGeometry(W_WIDTH - BTN_WIDTH - PADDING,
                        (W_HEIGHT - BTN_HEIGHT)/2, BTN_WIDTH, BTN_HEIGHT);

    connect(button, &QPushButton::clicked, [input, error_label, this](){HandleInput(input, error_label);});

}

int MainWindow::ProcessHack(const char* new_password, const char* filename)
{
    if (hack(new_password, filename)) {
        QLabel* error_label = new QLabel(background);
        error_label->setText("Can't open file!");
        error_label->setStyleSheet("QLabel { color: red; font-size: 16pt; }");
        error_label->setGeometry(W_WIDTH/2 - LABEL_WIDTH/2, W_HEIGHT/2 - error_label->height()/2,
                                 LABEL_WIDTH, error_label->height());
        error_label->show();
        return 1;
    }

    QLabel* success_label = new QLabel(background);
    success_label->setText("Hacked successfully!");
    success_label->setStyleSheet("QLabel { color: white; font-size: 18pt; }");
    success_label->setGeometry((W_WIDTH - 7./5 * LABEL_WIDTH)/2, (W_HEIGHT - LABEL_HEIGHT)/2,
                             LABEL_WIDTH * 7./5, LABEL_HEIGHT);
    success_label->show();
    return 0;
}

const char* MainWindow::GetPassword(QLineEdit* input)
{
    if (!(input->text()).length()) {

        return nullptr;
    }
    QString input_str = input->text();
    char* password = ToCStr(input_str);
    return password;
}

void MainWindow::HandleInput(QLineEdit* input, QLabel* error_label)
{
    const char* new_password = GetPassword(input);
    const char* filename = ToCStr(file_name);

    if (!strlen(filename)) {
        error_label->setText("No file specified");
        error_label->show();
        return;
    }
    if (new_password) {

        qDeleteAll(background->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));

        QMovie* progress = new QMovie(":/resources/img/progress_bar.gif");

        QLabel* progress_label = new QLabel(background);
        progress_label->setScaledContents(true);
        progress_label->setMovie(progress);
        progress_label->setGeometry((W_WIDTH - P_BAR_WIDTH)/2,
                                    (W_HEIGHT - P_BAR_HEIGHT)/2, P_BAR_WIDTH, P_BAR_HEIGHT);
        progress_label->show();

        connect(progress, &QMovie::frameChanged, [=](int frameNumber){
            if(frameNumber == progress->frameCount() - 1){
                progress->stop();
                progress_label->hide();
                ProcessHack(new_password, filename);
            }
        });

        progress->start();
        return;
    }

    error_label->setText("Password has to be at least one character long");
    error_label->show();

}

void MainWindow::OpenFile()
{
    file_name = QFileDialog::getOpenFileName(this, "Open file", "C:", "*.com");
}

char* ToCStr(QString str) {
    char* cstr = new char[str.length()]();
    strcpy(cstr, str.toStdString().c_str());
    return cstr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

