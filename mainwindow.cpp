#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    game = new Game;
    game->setFixedSize(game->DOT_WIDTH * game->FIELD_WIDTH, game->DOT_HEIGHT * game->FIELD_HEIGHT);

    game_bar = new QLabel(this);
    game_bar->setGeometry(0, 0, 600, 60);
    game_bar->setFixedSize(600, 60);
    game_bar->setStyleSheet("background-color: 'black'");
    game_bar->setStyleSheet("color: 'white'");
    game_bar->setFont(QFont("Myanmar Text", 38));
    game_bar->setAlignment(Qt::AlignLeft);


    //  Объединение виджета игры с таблицей счета над ней.
    layout = new QVBoxLayout(this);
    layout->addWidget(game_bar);
    layout->addWidget(game);
    ui->centralwidget->setLayout(layout);

    //  Выбор 10 уровней сложности на слайдере.
    ui->slider_difficulty->setMinimum(0);
    ui->slider_difficulty->setMaximum(10);
    ui->slider_difficulty->setTickPosition(QSlider::TicksBelow);
    ui->slider_difficulty->setTickInterval(1);

    this->setFixedSize(621, 641);

    hideAll();
    showMainMenu();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete game;
    delete layout;
    delete game_bar;
    delete STYLE_1;
    delete GAME_OVER_SCORE_TEXT;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Left && game->getDirection() != 1)  { game->setDirection(3); /*left*/  }
    if (key == Qt::Key_Right && game->getDirection() != 3) { game->setDirection(1); /*right*/ }
    if (key == Qt::Key_Up && game->getDirection() != 2)    { game->setDirection(0); /*up   */ }
    if (key == Qt::Key_Down && game->getDirection() != 0)  { game->setDirection(2); /*down */ }
}

void MainWindow::timerEvent(QTimerEvent *)
{
    if (!game->isInGame())
    {
        killTimer(timer);
        this->gameOver();
    }
    game_bar->setText(std::to_string(game->getScore()).c_str());
}

int MainWindow::checkRecordFile()
{
    //  Открытие файла с рекордом.
    QFile record_file("./record.txt");  //  ! НЕОБХОДИМО ДОБАВИТЬ ФАЙЛ record.txt В ДИРЕКТОРИЮ СБОРКИ !.

    if (!record_file.open(QFile::ReadOnly | QIODevice::Text)) { return -1; }
    QString record_str = record_file.readLine();
    int record;

    //  Не пустой ли файл.
    if (record_str.size() < 1)
    {
        //  Записываем 0, если пустой.
        if (record_file.isOpen()) { record_file.close(); }

        if (!record_file.open(QFile::WriteOnly | QIODevice::Text)) { return -2; }

        QTextStream out(&record_file);

        out << "0";

        record = 0;
    }
    else
    {
        try
        {
            record = std::stoi(record_str.toStdString().c_str());
        }  catch (std::exception e) { return -3; }
    }

    //  Если рекорд побит.
    if (game->getScore() > record)
    {
        if (record_file.isOpen()) { record_file.close(); }

        if (!record_file.open(QFile::WriteOnly | QIODevice::Text)) { return -4; }

        QTextStream out(&record_file);

        out << std::to_string(game->getScore()).c_str();
        record = game->getScore();
    }

    if (record_file.isOpen()) { record_file.close(); }

    return record;
}

void MainWindow::gameOver()
{
    hideAll();

    int record = checkRecordFile();  //  Получение и проверка рекорда.

    ui->label_game_over_score->setText(GAME_OVER_SCORE_TEXT);

    //  Вывод набранных очков.
    //  Позиция для счета в label - 15.
    ui->label_game_over_score->setText(ui->label_game_over_score->text().insert(15, std::to_string(game->getScore()).c_str()));

    ui->label_game_over_score->setText(ui->label_game_over_score->text() + std::to_string(record).c_str());
    ui->centralwidget->setStyleSheet(STYLE_1);
    ui->label_game_over->show();
    ui->label_game_over_score->show();
    ui->button_restart->show();
    ui->button_go_main_menu->show();
}

void MainWindow::hideAll()
{
    ui->centralwidget->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(0, 151, 119, 255), stop:1 rgba(0, 34, 142, 255))");
    //  Main menu.
    ui->title_label->hide();
    ui->title_label_2->hide();
    ui->button_start_game->hide();
    ui->button_options->hide();
    ui->button_help->hide();
    ui->button_exit->hide();

    //  Game.
    game->hide();
    ui->label_game_over->hide();
    ui->label_game_over_score->hide();
    ui->button_restart->hide();
    ui->button_go_main_menu->hide();
    game_bar->hide();

    //  Options.
    ui->button_go_main_opt->hide();
    ui->button_set_default->hide();
    ui->button_reset_stat->hide();
    ui->slider_difficulty->hide();
    ui->label_difficulty->hide();
}

void MainWindow::showMainMenu()
{
    //  Main menu.
    ui->title_label->show();
    ui->title_label_2->show();
    ui->button_start_game->show();
    ui->button_options->show();
    ui->button_help->show();
    ui->button_exit->show();
}


void MainWindow::on_button_start_game_clicked()
{
    ui->centralwidget->setFocusPolicy(Qt::StrongFocus);  //  Установка фокусировки на клавиатуру.
    hideAll();

    ui->centralwidget->setStyleSheet(STYLE_1);
    game->show();
    game_bar->show();

    game_bar->setText("0");
    game_bar->show();

    game->initGame();
    timer = startTimer(game->getDelay());
}


void MainWindow::on_button_go_main_menu_clicked()
{
    hideAll();
    showMainMenu();
}


void MainWindow::on_button_restart_clicked()
{
    hideAll();
    on_button_start_game_clicked();
}


void MainWindow::on_button_exit_clicked()
{
    exit(0);
}

//  Переход в раздел "Настройки"
void MainWindow::on_button_options_clicked()
{
    hideAll();

    //  Title.
    ui->title_label->show();
    ui->title_label_2->show();

    ui->button_go_main_opt->show();
    ui->button_set_default->show();
    ui->button_reset_stat->show();
    ui->slider_difficulty->show();
    ui->label_difficulty->show();
}


void MainWindow::on_button_go_main_opt_clicked()
{
    game->setDelay(600 / (ui->slider_difficulty->value() + 1));
    qDebug() << game->getDelay();
    on_button_go_main_menu_clicked();
}


void MainWindow::on_button_set_default_clicked()
{
    ui->slider_difficulty->setValue(3);
}

void MainWindow::on_button_reset_stat_clicked()
{

}

void MainWindow::on_button_help_clicked()
{
    QMessageBox::StandardButton reply;

    reply = QMessageBox::question(this, "как это все работает", "это легендарная игра с простым принципом: игрок управляет змеей, которая неустанно движется вперед по клетке. Если он коснется стен или откусит себе хвост, он умрет, и игра закончится. Чтобы заработать очки, он должен проглатывать бонусы, которые заставляют его расти, пока его размер не станет проблемой. Концепция немного рудиментарна  если эти информации были полезни для вас?" , QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
       QMessageBox::information(this, "тест", "тогда проверим что вы поняли");
        hideAll();
        on_button_start_game_clicked();}

    else {QMessageBox::information(this, "совет", "я посоветую вам сначала выбрать самый низкий уровень и протестировать игру самостоятельно.мы уверень что вы будете поражены простотой нашей игры.");
           ui->slider_difficulty->setValue(1);
           on_button_start_game_clicked();
    }


}
