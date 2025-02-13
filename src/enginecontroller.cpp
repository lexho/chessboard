#include "enginecontroller.h"
#include "ui_enginecontroller.h"
#include "qdialvalue.h"
#include <QListWidgetItem>
#include <QDebug>

EngineController::EngineController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EngineController)
{
    ui->setupUi(this);
    connect(ui->engineSelect, SIGNAL(activated(int)), this, SLOT(selectEngine(int)));

    /* Create Engines */
    uci = new UCIEngine("/bin/stockfish");
    chess = new ChessEngine("/home/alex/cpp/giraffe/giraffe");
    engine = NULL;
    selectEngine(0); // set default engine

    connect(ui->goButton, SIGNAL(pressed()), this, SLOT(toggleGoStop()));
    goButtonPressed = false;

    /* Link QDials and QSpinBoxes */
    connect(ui->spinBox_depth, SIGNAL(valueChanged(int)), ui->searchdepth, SLOT(setValue(int)));
    connect(ui->spinBox_movetime, SIGNAL(valueChanged(int)), ui->movetime, SLOT(setValue(int)));
    connect(ui->spinBox_nodes, SIGNAL(valueChanged(int)), ui->nodes, SLOT(setValue(int)));
    connect(ui->searchdepth, SIGNAL(valueChanged(int)), ui->spinBox_depth, SLOT(setValue(int)));
    connect(ui->movetime, SIGNAL(valueChanged(int)), ui->spinBox_movetime, SLOT(setValue(int)));
    connect(ui->nodes, SIGNAL(valueChanged(int)), ui->spinBox_nodes, SLOT(setValue(int)));

    connect(ui->playButton, SIGNAL(pressed()), this, SLOT(play()));

    connect(ui->radio_play, SIGNAL(pressed()), this, SLOT(turnOn()));
    connect(ui->radio_think, SIGNAL(pressed()), this, SLOT(turnOn()));
    connect(ui->radio_power, SIGNAL(pressed()), this, SLOT(turnOff()));

    ui->searchdepth->setMinimum(0);
    ui->searchdepth->setMaximum(25);
    ui->movetime->setMinimum(0);
    //ui->movetime->setMaximum(100000);
    ui->nodes->setMinimum(0);
    ui->nodes->setMaximum(500);
    ui->spinBox_depth->setMaximum(ui->searchdepth->maximum());
    ui->spinBox_movetime->setMaximum(ui->movetime->maximum());
    ui->spinBox_nodes->setMaximum(ui->nodes->maximum());
    turnOff();

    //ui->verticalLayout->addWidget(uci->output);
    //ui->verticalLayout->addWidget(chess->output);
}

EngineController::~EngineController()
{
    delete ui;
}

bool EngineController::isOn() {
    return !ui->radio_power->isChecked();
}

void EngineController::turnOff() {
    ui->goButton->setEnabled(false);
    ui->playButton->setEnabled(false);
    ui->spinBox_depth->setEnabled(false);
    ui->spinBox_movetime->setEnabled(false);
    ui->spinBox_nodes->setEnabled(false);
    ui->searchdepth->setEnabled(false);
    ui->movetime->setEnabled(false);
    ui->nodes->setEnabled(false);
    ui->progressBar->setEnabled(false);
    ui->bestmove->setEnabled(false);
    ui->listOtherMoves->setEnabled(false);
    //engine->output->setEnabled(false);
}

void EngineController::turnOn() {
    ui->goButton->setEnabled(true);
    ui->playButton->setEnabled(true);
    ui->spinBox_depth->setEnabled(true);
    ui->spinBox_movetime->setEnabled(true);
    ui->spinBox_nodes->setEnabled(true);
    ui->searchdepth->setEnabled(true);
    ui->movetime->setEnabled(true);
    ui->nodes->setEnabled(true);
    ui->progressBar->setEnabled(true);
    ui->bestmove->setEnabled(true);
    ui->listOtherMoves->setEnabled(true);
    engine->output->setEnabled(true);
}

void EngineController::setBoard(Board *board) {
    this->board = board;
}

void EngineController::setGame(Game* game) {
    this->game = game;
}

void EngineController::go() {
    /*qDebug() << "EngineController:" << "go";
    if(isOn()) qDebug() << "EngineController:" << "is on";
    if(goButtonPressed) qDebug() << "EngineController:" << "Go-Button is pressed";
    if(ui->radio_play->isChecked()) qDebug() << "EngineController:" << "Radio Play is checked";
    if(ui->checkBox_black->isChecked()) qDebug() << "EngineController:" << "Black-Checkbox is checked";
    if(ui->checkBox_black->isChecked() && !ui->checkBox_white->isChecked() && game->getActiveColor() == 'w') qCritical() << "EngineController:" << "go error";
    if(engine->isThinking()) qDebug() << "EngineController:" << "is thinking";*/

    if(isOn()) {
        if(!goButtonPressed) {
            if(ui->radio_play->isChecked()) {
                if(ui->checkBox_black->isChecked() && !ui->checkBox_white->isChecked() && game->getActiveColor() == 'w') return;
                if(ui->checkBox_white->isChecked() && !ui->checkBox_black->isChecked() && game->getActiveColor() == 'b') return;
            }
        }
        if(!engine->isThinking()) {
            engine->setPosition(game->board->getFenstring());
            activeColor = game->board->getActiveColor();
            engine->go();
            ui->goButton->setText("stop");
            ui->playButton->setEnabled(false);
            ui->progressBar->setValue(0);
        }
        goButtonPressed = false;
    }
}

void EngineController::toggleGoStop() {
    //qDebug() << "toggle go stop";
    if(isOn()) {
        if(engine->isThinking()) {
            engine->stop();
            ui->goButton->setText("stopping");
        } else {
            goButtonPressed = true;
            go();
        }
    }
}

void EngineController::play() {
    qDebug() << "EngineController::play";
    /* make moves in long algebraic notation "d2d4" or "f7f8q" */
    if(engine->getBestmove().size() == 4 || engine->getBestmove().size() == 5) {
        game->move(engine->getBestmove());

        /* Let other widgets know, that we made a move */
        cout << "Made move" << endl;
        emit madeMove();
        ui->playButton->setDisabled(true);

        /* Think on next move if radiobutton play and color checkbox are checked */
        cout << "Think on next move" << endl;
        if(ui->radio_play->isChecked() || ui->radio_think->isChecked()) {
            if(game->getActiveColor() == 'b') selectEngine(1);
            else selectEngine(0);
            if((ui->checkBox_black->isChecked() && game->getActiveColor() == 'b')
                    || (ui->checkBox_white->isChecked() && game->getActiveColor() == 'w')) go();
        }
    }
}

void EngineController::showOtherMoves() {
    vector<vector<string>> moves = engine->getOtherMoves();
    if(moves.size() == 0) return;
    string txt;
    /* Add moves to listOtherMoves widget */
    ui->listOtherMoves->clear();
    for(int i = 0; i < moves.size(); i++) {
        //string line = "<b>" + moves[i].substr(0, 4) + "</b>" + moves[i].substr(4) + "<br />";
        //txt.append(line);
        new QListWidgetItem(QString::fromStdString(moves[i][0].substr(0, 5*8) + " " + moves[i][1]), ui->listOtherMoves);
    }
    connect(ui->listOtherMoves, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(showMove(QListWidgetItem*)));
    //ui->otherMoves->setText(QString::fromStdString(txt));
    ui->bm_score->setText(QString::fromStdString(moves[0][1]));
    emit newBestmoveScore(getBestmoveScore());
}

void EngineController::showMove(QListWidgetItem* item) {
    string str = item->text().toStdString();
    emit newBestmove(str.substr(0,4));
}

void EngineController::showMove(string move) {
}

void EngineController::showBestmove() {
    /* Write Bestmove to Screen */
    ui->bestmove->setText(QString::fromStdString(engine->getBestmove()));
    ui->progressBar->setValue(100);

    /* Make a move or activate play button */
    if(ui->radio_play->isChecked()) {
        qDebug() << "play is checked";
        if(ui->checkBox_black->isChecked() && game->getActiveColor() == 'b') play();
        if(ui->checkBox_white->isChecked() && game->getActiveColor() == 'w') play();
    } else
    ui->playButton->setEnabled(true);
    string bestmove = engine->getBestmove();
    //qDebug() << "EngineController" << "bestmove " << bestmove << endl;
    if(bestmove != "(none)\n") {
        emit newBestmove(bestmove);
    }
}

void EngineController::showDepth(int depth) {
    double x = (double) depth/ (double) ui->searchdepth->value() * 100;
    double fx = pow(2.5,(x/10 - 5));
    if((int)fx > ui->progressBar->value())
    ui->progressBar->setValue((int)fx);
}

void EngineController::updateController(int state) {
    qDebug() << "EngineController:" << "update controller state" << state;
    switch (state) {
    case 0:
        ui->goButton->setText("go");
        break;
    case 1:
        ui->goButton->setText("stop");
        break;
    }
}

void EngineController::undock() {
    setParent(0);
    show();
}

void EngineController::selectEngine(int index) {
    qDebug() << "EngineController:" << "selecting Engine" << index;
    ui->engineSelect->setCurrentIndex(index);

    ui->searchdepth->disconnect(engine);
    ui->movetime->disconnect(engine);
    ui->nodes->disconnect(engine);


    if(index == 0) {
        engine = uci;
        uci->output->show();
    } else {
        engine = chess;
        chess->output->show();
    }

    //ui->verticalLayout->addWidget(engine->output);

    /* Set controllers to engine values */
    cout << "depth: " << engine->getSearchDepth() << ", movetime: " << engine->getMovetime() << ", nodes: " << engine->getNodes() << endl;
    ui->searchdepth->setValue(engine->getSearchDepth());
    ui->movetime->setValue(engine->getMovetime());
    ui->nodes->setValue(engine->getNodes());

    /*engine->setSearchDepth(ui->searchdepth->value());
    engine->setMovetime(ui->movetime->value());
    engine->setNodes(ui->nodes->value());*/

    connect(engine, SIGNAL(newBestmove()), this, SLOT(showBestmove()));
    connect(engine, SIGNAL(newBestmove()), this, SLOT(showOtherMoves()));
    connect(engine, SIGNAL(stateChanged(int)), this, SLOT(updateController(int)));
    connect(engine, SIGNAL(newDepth(int)), this, SLOT(showDepth(int)));

    /* Connect Search Parameter Controllers to Engine */
    connect(ui->searchdepth, SIGNAL(valueChanged(int)), engine, SLOT(setSearchDepth(int)));
    connect(ui->movetime, SIGNAL(valueChanged(int)), engine, SLOT(setMovetime(int)));
    connect(ui->nodes, SIGNAL(valueChanged(int)), engine, SLOT(setNodes(int)));
}

int EngineController::getBestmoveScore() {
    int bmScore = ui->bm_score->text().toInt();
    if(activeColor == 'w') bmScore *= -1;
    return bmScore;
}
