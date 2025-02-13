#include "game.h"
#include <boost/lexical_cast.hpp>
#include <QDebug>

size_t Game::NrOfGames = 0;

Game::Game()
{
    //board = new Board(Fen ());
    board = new QBoard();
    NrOfGames++;
    movehistory = new QTreeWidget();
    movehistory->setWindowTitle("move list");
    //movehistory->setFixedWidth(150);
    movehistory->setMinimumWidth(150);
    movehistory->setMaximumWidth(150);
    movehistory->setColumnCount(3);
    movehistory->setColumnWidth(0, 25);
    for(int i = 1; i < movehistory->columnCount(); i++) {
        movehistory->setColumnWidth(i, 50);
    }
    movehistory->setFocusPolicy(Qt::NoFocus);
    movehistory->headerItem()->setText(0, "#");
    movehistory->headerItem()->setText(1, "white");
    movehistory->headerItem()->setText(2, "black");
    connect(movehistory, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(HistoryItemClicked(QTreeWidgetItem*, int)));
    connect(board, SIGNAL(madeMove()), this, SLOT(showMoveHistory()));
}

Game::~Game() {
    cout << "destroy Game " << gameID << endl;
    delete movehistory;
    delete board;
}

size_t Game::getNrOfGames() {
    return NrOfGames;
}

int Game::getGameID() {
    return gameID;
}

void Game::setGameID(int id) {
    gameID = id;
}

char Game::getActiveColor() {
    if(board->getActiveColor() % 2 == 0) activeColor = 'b';
    else activeColor = 'w';
    return activeColor;
}

void Game::setActiveColor(char c) {
    activeColor = c;
    board->setActiveColor(c);
}

/* Update Movehistory */
void Game::showMoveHistory() {
    vector<string> mvhist = board->getMoveHistory();
    if(mvhist.size() == 0) return;
    //cout << "currmoveNr" << currmoveNr << endl;
    movehistory->clear();
    for(int i = 0; i < mvhist.size(); i++) {
        if(i % 2 == 0) movehistory->insertTopLevelItem(i/2, new QTreeWidgetItem());
        movehistory->topLevelItem(i / 2)->setText(0, QString::fromStdString(boost::lexical_cast<string>(i/2)));
        movehistory->topLevelItem(i / 2)->setText(1 + i % 2, QString::fromStdString(mvhist[i]));
    }
    //currmoveNr = mvhist.size();
    for(int i = 0; i < movehistory->columnCount(); i++) {
        movehistory->resizeColumnToContents(i);
    }
    //movehistory->show();
    //cout << floor(currmoveNr/2) << endl;
    qDebug() << "movehistory items: " << movehistory->topLevelItemCount();
    qDebug() << "select movehistoryitem: " << floor(currmoveNr/2);
    if(floor(currmoveNr/2) < movehistory->topLevelItemCount()) {
        if(movehistory->topLevelItem(floor(currmoveNr/2)) != NULL)
            movehistory->topLevelItem(floor(currmoveNr/2))->setSelected(true);
    }
}

void Game::move(string movecmd) {
    board->QBoard::move(movecmd);
    board->show();
    showMoveHistory();
    currmoveNr++;
}

void Game::nextPos() {
    board->nextPos();
    board->show();
    vector<string> mvhist = board->getMoveHistory();
    if (currmoveNr + 1 < mvhist.size()) currmoveNr++;
    showMoveHistory();
}

void Game::nextPos(int index) {
    board->nextPos(index);
    board->show();
    vector<string> mvhist = board->getMoveHistory();
    if (currmoveNr + 1 < mvhist.size()) currmoveNr++;
    showMoveHistory();
}

void Game::prevPos() {
    board->prevPos();
    board->show();
    if(currmoveNr > 0) currmoveNr--;
    showMoveHistory();
}

void Game::HistoryItemClicked(QTreeWidgetItem* item, int c) {
    int index;
    index = item->text(0).toInt() * 2 + (c - 1);
    cout << index << " ";
    cout << item->text(c).toStdString() << endl;
    board->setPosition(index);
    board->show();
    currmoveNr = index;
}

void Game::saveGame() {
    board->saveGame();
}

int Game::getCurrentMoveNr() {
    return currmoveNr;
}

void Game::setTimeW(int time) {
    board->setTimeW(time);
}

void Game::setTimeB(int time) {
    board->setTimeB(time);
}

void Game::show() {
    board->playerW->show();
    board->playerB->show();
    board->playerPhotoW->show();
    board->playerPhotoB->show();
    board->timeLabelW->show();
    board->timeLabelB->show();
    board->Slider->show();
    movehistory->show();
}

void Game::hide() {
    board->timeLabelW->hide();
    board->timeLabelB->hide();
    board->playerW->hide();
    board->playerB->hide();
    board->playerPhotoW->hide();
    board->playerPhotoB->hide();
}
