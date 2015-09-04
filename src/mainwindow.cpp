#include "mainwindow.h"
#include <QtCore>
#include <QtGui>
#include "fen.h"
#include <QAction>
#include"uciengine.h"

MainWindow::MainWindow(QMainWindow *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags) {

    /* Boolean Variables */
    chessserver = false;
    getICGameList = false;
    chessengine = false;
    localboard = true;
    thinkOnMove = false;
    engineB = false;
    engineW = false;
    moved = false;


    this->setWindowTitle("Chessboard 0.6");
    this->resize(1200, 400);

    engineController = new EngineController();

    dialog = new DBdialog(myChessDB);
    dialog->setAttribute(Qt::WA_QuitOnClose);
    QObject::connect(dialog, SIGNAL(finished(int)), this, SLOT(checkInputDialog(int)));
    QObject::connect(dialog, SIGNAL(rejected()), this, SLOT(checkInputDialog()));

    fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);
    editMenu = new QMenu(tr("&Edit"), this);
    menuBar()->addMenu(editMenu);
    gameMenu = new QMenu(tr("&Game"), this);
    menuBar()->addMenu(gameMenu);
    engineMenu = new QMenu(tr("&Engine"), this);
    menuBar()->addMenu(engineMenu);
    icsMenu = new QMenu(tr("&ICS"), this);
    menuBar()->addMenu(icsMenu);
    viewMenu = new QMenu(tr("&View"), this);
    menuBar()->addMenu(viewMenu);

    fileMenu->addAction( QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("New Game"), this, SLOT(newGame()), QKeySequence(tr("Ctrl+N", "File|New Game")));
    fileMenu->addAction( QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("Load Game From &Database"), dialog, SLOT(exec()), QKeySequence(tr("Ctrl+D", "File|Database")));
    fileMenu->addAction( QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("Quit"), this, SLOT(quit()), QKeySequence(tr("Ctrl+Q", "Quit")));
    engineMenu->addAction( QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("&Think on position"), this, SLOT(think()), QKeySequence(tr("Ctrl+T", "Engine|Think")));
    engineMenu->addAction( QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("Play &Black"), this, SLOT(EnginePlayBlack()), QKeySequence(tr("Ctrl+B", "Engine|Play Black")));
    engineMenu->addAction( QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("Play &White"), this, SLOT(EnginePlayWhite()), QKeySequence(tr("Ctrl+B", "Engine|Play White")));

    icsMenu->addAction( QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("&Connect with Internet Chess Server"), this, SLOT(ICSconnect()), QKeySequence(tr("Ctrl+S", "ICS|Server")));
    icsMenu->addAction( QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("Games on Server"), this, SLOT(ICSgameList()), QKeySequence(tr("Ctrl+S", "ICS|Server")));

    gameMenu->addAction(QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("&Flip View"), this, SLOT(), QKeySequence(tr("Ctrl+F", "Game|Flip View")));
    gameMenu->addAction(QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("&Next Move"), this, SLOT(nextPos()));
    gameMenu->addAction(QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("&Previous Move"), this, SLOT(prevPos()));
    gameMenu->addAction(QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("Set Game ID"), this, SLOT(setGameID()));
    gameMenu->addAction(QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("Set Active Color"), this, SLOT(setActiveColor()));
    gameMenu->addAction(QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("Duplicate Game"), this, SLOT());
    gameMenu->addAction(QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("Undock Engine-Controller"), engineController, SLOT(undock()));

    viewMenu->addAction(QIcon(QString("%1%2") .arg(QCoreApplication::applicationDirPath()) .arg("/images/page_white.png")), tr("Undock Gameinfo"), this, SLOT(undock()));

    /* Initialize Games and Boards */
    for(int i = 0; i < NrOfBoards; i++) {
        game.push_back(new Game());
        connect(game[i]->board, SIGNAL(madeMove()), engineController, SLOT(go()));
        /*for(int j = 0; j < game[i]->board->squares.size(); j++) {
            QObject::connect(game[i]->board->squares[j], SIGNAL(clicked(int)), this, SLOT(SquareClicked(int)));
            QObject::connect(game[i]->board->squares[j], SIGNAL(dropped(int, int)), this, SLOT(SquareDropped(int, int)));
        }*/
    }

    posID = 1;

    /* Buttons */
    next = new QPushButton("next");
    nextCombo = new QComboBox;
    nextCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    nextCombo->hide();
    QObject::connect(nextCombo, SIGNAL(activated(int)), this, SLOT(setNextPosition(int)));
    //nextCombo->addItem("All Events");
    back = new QPushButton("back");
    input = new QLineEdit();
    EngineButton = new QPushButton("Think");
    for(int i = 0; i < NrOfButtons; i++) {
        button.push_back(new QPushButton(QString("B").append(QString(QString::fromStdString(boost::lexical_cast<string>(i))))));
        if(i != 2) button[i]->setFixedSize(40,25);
    };
    EngineButton->setFixedSize(50,25);
    button[4]->setText("<");
    button[5]->setText(">");
    button[0]->setText("R ICS");
    button[1]->setText("games");
    button[2]->setText("Pos Tree");
    button[3]->setText("Scan ICS");
    button[6]->setText("Bestmove");


    /* Signal - Slot Connections */  
    connect(input, SIGNAL(returnPressed()), this, SLOT(readInput()));

    /* Buttons */
    connect(next, SIGNAL(clicked()), this, SLOT(nextPos()));
    connect(back, SIGNAL(clicked()), this, SLOT(prevPos()));
    connect(EngineButton, SIGNAL(clicked()), this, SLOT(think()));
    connect(button[0], SIGNAL(clicked()), this, SLOT(readICServer()));
    connect(button[1], SIGNAL(clicked()), this, SLOT(ICSgameList()));
    connect(button[2], SIGNAL(clicked()), this, SLOT(showPositionTree()));
    connect(button[3], SIGNAL(clicked()), this, SLOT(scanICS()));
    connect(button[4], SIGNAL(clicked()), this, SLOT(prevPos()));
    connect(button[5], SIGNAL(clicked()), this, SLOT(nextPos()));
    connect(button[6], SIGNAL(clicked()), &engine, SLOT(showOutput()));

    connect(&engine, SIGNAL(newBestmove()), this, SLOT(showBestmoveMessage()));

    /* Status Updates */
    connect(button[4], SIGNAL(clicked()), this, SLOT(updateStatusBar()));
    connect(button[5], SIGNAL(clicked()), this, SLOT(updateStatusBar()));
    connect(engineController, SIGNAL(madeMove()), this, SLOT(updateStatusBar()));

/* -------------------------------------------------------------------------------------- */
/*   D e f i n e  L a y o u t                                                             */
/* -------------------------------------------------------------------------------------- */

    /* Tabbed Board - Layout */
    BoardTab = new QTabWidget();
    QGroupBox* b = new QGroupBox;
    b->setFixedSize(300,300);
    for(int i = 0; i < Game::getNrOfGames(); i++) {
        BoardBox.push_back(new QGroupBox);
        BoardBox[i]->setFixedSize(576,576);
        BoardTab->addTab(BoardBox[i], "Game " + QString::fromStdString(boost::lexical_cast<string>(i + 1)));
        BoardBox[i]->setLayout(game[i]->board->Grid);
    }
    QObject::connect(BoardTab, SIGNAL(currentChanged(int)), this, SLOT(setBoardActive(int)));

    /* Buttons */
    ButtonFrame = new QFrame();
    ButtonBoxLayout = new QHBoxLayout;
    ButtonFrame->setLayout(ButtonBoxLayout);
    ButtonBoxLayout->addWidget(EngineButton);
    for(int i = 0; i < NrOfButtons; i++) {
        ButtonBoxLayout->addWidget(button[i]);
    }
    ButtonBoxLayout->addWidget(nextCombo);

    QImage img("./person.png");
    QImage img2("./person.png");

    /* Player Info */
    for(int i = 0; i < 2; i++) {
        playerFrame.push_back(new QFrame());
        playerLayout.push_back(new QHBoxLayout);
        player.push_back(new PlayerLabel);
        player[i]->setText("Player " + boost::lexical_cast<string>(1-i));
        time.push_back(new TimeLabel);
        score.push_back(new QLabel);

        /* Player Photo */
        QLabel* playerPhoto = new QLabel();
        playerPhoto->setAlignment(Qt::AlignCenter);
        if(i == 0)
            playerPhoto->setPixmap(QPixmap::fromImage(img2.scaledToWidth(64)));
        else
            playerPhoto->setPixmap(QPixmap::fromImage(img.scaledToWidth(64)));
        //playerPhoto->setStyleSheet("border: 2px solid #ffff00");
        playerLayout[i]->addWidget(playerPhoto);

        playerLayout[i]->addWidget(player[i]);
        playerLayout[i]->addWidget(time[i]);
        playerLayout[i]->addWidget(score[i]);
        playerFrame[i]->setLayout(playerLayout[i]);  
    }

    for(int i = 0; i < player.size(); i++) {
        connect(player[i], SIGNAL( clicked(int) ), this, SLOT( setPlayerName(int) ) );
    }

    /* Style Player and Time Labels */
    player[0]->setColor("black");
    player[1]->setColor("white");
    time[0]->setColor("black");
    time[1]->setColor("white");
    time[0]->setTime(1800);
    for(int i = 0; i < 2 * Game::getNrOfGames(); i++) {
        players.push_back("Player " + boost::lexical_cast<string>(i));
        t.push_back(1800);
    }

    player[0]->setText(players[activeBoard * 2 + 1]);
    player[0]->setID(activeBoard * 2 + 1);
    player[1]->setText(players[activeBoard * 2]);
    player[1]->setID(activeBoard * 2);

    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(clocks()));

    timer->start(1000);

    /* Output - Boxes */
    output = new QTextEdit();
    output->setReadOnly(true);
    output->hide();
    engineView = new QWebView();
    QObject::connect(engineView, SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl)));

    /* Game Info */
    gameinfo = new QWidget;
    QVBoxLayout* gameinfoLayout = new QVBoxLayout;
    gameinfo->setLayout(gameinfoLayout);
    gameinfoLayout->addWidget(playerFrame[0]);
    gameinfoLayout->addWidget(playerFrame[1]);

    GameInfoLayout = new QVBoxLayout;
    GameInfoLayout->addWidget(gameinfo);
    //GameInfoLayout->addWidget(playerFrame[0]);
    //GameInfoLayout->addWidget(playerFrame[1]);
    GameInfoLayout->addWidget(ButtonFrame);
    /*for(int i = 0; i < NrOfBoards; i++) {
        GameInfoLayout->addWidget(game[i]->board->Slider); // Game-Progress-Slider
        game[i]->board->Slider->hide();
    }*/
    //GameInfoLayout->addWidget(input);
    GameInfoLayout->addWidget(output);
    QGroupBox* engineControllerGroup = new QGroupBox();
    QVBoxLayout* engineControllerLayout = new QVBoxLayout();
    engineControllerGroup->setLayout(engineControllerLayout);
    QString style = "QGroupBox {border: 1px solid gray;border-radius: 9px;margin-top: 0.5em;} QGroupBox::title {subcontrol-origin: margin;left: 10px;padding: 0 3px 0 3px;}";
    engineControllerGroup->setStyleSheet(style);
    engineControllerGroup->setTitle("Engine Controller");
    engineControllerLayout->addWidget(engineController);
    GameInfoLayout->addWidget(engineControllerGroup);
    GameInfoBox = new QGroupBox("Game Info");
    GameInfoBox->setLayout(GameInfoLayout);
    QVBoxLayout* boardSliderBox = new QVBoxLayout();
    boardSliderBox->addWidget(BoardTab);
    for(int i = 0; i < NrOfBoards; i++) {
        boardSliderBox->addWidget(game[i]->board->Slider); // Game-Progress-Slider
        game[i]->board->Slider->hide();
    }


    /* Main Layout */
    layout = new QHBoxLayout;
    layout->addLayout(boardSliderBox);
    layout->addWidget(GameInfoBox);
    for(int i = 0; i < game.size(); i++) {
        layout->addWidget(game[i]->movehistory);
        game[i]->movehistory->hide();
    }

    layout->setMargin(0);

    centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    this->setCentralWidget(centralWidget);
    this->setFocusPolicy(Qt::StrongFocus);
    statusBar();
    statusActiveColor = new QLabel();
    statusActiveColor->setText(QString::fromStdString(boost::lexical_cast<string>(game[activeBoard]->getActiveColor())));
    statusMoveNr = new QLabel();
    statusMoveNr->setText(QString::fromStdString(boost::lexical_cast<string>(game[activeBoard]->getCurrentMoveNr())));
    statusBar()->addPermanentWidget(statusMoveNr);
    statusBar()->addPermanentWidget(statusActiveColor);

    game[activeBoard]->board->Slider->show();
    game[activeBoard]->movehistory->show();

/* -------------------------------------------------------------------------------------- */


    /* Get position data from Database and display it on the GUI board */
    for(int i = 0; i < Game::getNrOfGames(); i++) {
        //game[i]->board->getPositionFromDBByID(posID);
        game[i]->board->show(); // Write position to squares (QLabels)
        posIDs.push_back(vector<int> ());
        posIndex.push_back(0);
    }

    /* Connect Engine-Thread with Output-TextEdit Object */
    //QObject::connect(&engine, SIGNAL(newOutput()), this, SLOT(printEngineOutput()));

    //engine.start();
    //engine.stockfish();
    engineController->show();
    engineController->setGame(game[0]);

    QObject::connect(&fics, SIGNAL(unread()), this, SLOT(readICServer()));

}  
void MainWindow::setBoardActive(int index) {
    cout << "board " + boost::lexical_cast<string>(index) + " set active" << endl;
    //game[activeBoard]->movehistory->setParent(0);
    activeBoard = index;
    //cout << players[activeBoard * 2] << endl;
    //cout << players[activeBoard * 2 + 1] << endl;

    player[0]->setText(players[activeBoard * 2 + 1]);
    //player[0]->setID(activeBoard * 2);
    player[1]->setText(players[activeBoard * 2]);
    //player[1]->setID(activeBoard * 2 + 1);
    time[1]->setTime(t[activeBoard * 2 + 1]);
    time[0]->setTime(t[activeBoard * 2]);
    /*time[0]->setText("<font size=20 color=black><b>" + QString::fromStdString(makeTime(t[activeBoard * 2 + 1])) + "</b></font>");
    time[1]->setText("<font size=20 color=white><b>" + QString::fromStdString(makeTime(t[activeBoard * 2])) + "</b></font>");*/
    game[activeBoard]->board->show();
    engineController->setGame(game[index]);

    /* Hide Movehistory and Slider of inactive games */
    for(int i = 0; i < NrOfBoards; i++) {
        game[i]->board->Slider->hide(); /* Game-Progress-Slider */
        game[i]->movehistory->hide();
    }
    game[activeBoard]->board->Slider->show();
    game[activeBoard]->movehistory->show();
    updateStatusBar();
}

void MainWindow::checkInputDialog() {
    int val = dialog->result();
    if( val == QDialog::Accepted) {
        cout << "Akzeptiert!" << endl;
    } else if (val == QDialog::Rejected) {
        cout << "Abgelehnt!" << endl;
    }
}

void MainWindow::checkInputDialog(int gameID) {
    cout << "checkInputDialog " << gameID << endl;
    if (!dialog->result() == QDialog::Rejected) {
        localboard = true;
        game[activeBoard]->board->loadGame(gameID);
        cout << "game loaded" << endl;

        vector<string> p = myChessDB.getPlayersByGameID(gameID);
        for(int i = 0; i < p.size(); i++) {
            players[activeBoard * 2 + i] = p[i];
        }
        player[0]->setText(players[activeBoard * 2]);
        player[1]->setText(players[activeBoard * 2 + 1]);

        //updateBoard();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
   switch(e->key()) {
   case Qt::Key_Right:
       nextPos();
       break;
   case Qt::Key_Left:
       prevPos();
       break;
   }
   updateStatusBar();
}

void MainWindow::nextPos() {
    if(chessserver && examining && !localboard) {
       fics.writeSocket("forward\n");
    } else {
        game[activeBoard]->nextPos(nextPosIndex);
        getNextPositions();
        //game[activeBoard]->board->show();
        /*if(posIDs[activeBoard].size() > 0 && (posIndex[activeBoard] + 1) < posIDs[activeBoard].size()) {
            posIndex[activeBoard]++;
            updateBoard();
        }*/
    }
    if(thinkOnMove) think();
}

void MainWindow::getNextPositions() {
    nextPosIndex = 0;
    if(game[activeBoard]->board->hasChildren()) {
        nextCombo->clear();
        vector<Fen*> nextPositions = game[activeBoard]->board->getChildren();
        if(nextPositions.size() > 1) {
            for(int i = 0; i < nextPositions.size(); i++) {
                nextCombo->addItem(QString::fromStdString(boost::lexical_cast<string>(myChessDB.getPositionIDFromDB(*nextPositions[i]))));
            }
            nextCombo->show();
        } else nextCombo->hide();
    }
}

void MainWindow::prevPos() {
    if(chessserver && examining && !localboard) {
       fics.writeSocket("backward\n");
    } else {
        unsetEngine(0);
        unsetEngine(1);
        game[activeBoard]->prevPos();
        getNextPositions();
        /*if(posIndex[activeBoard]>0) {
            posIndex[activeBoard]--;
            updateBoard();
        }*/
    }
    if(thinkOnMove) think();
}

/* Engine Slots                                                               */
/* -------------------------------------------------------------------------- */

void MainWindow::EnginePlayBlack() {
   engineB = true;
   if(game[activeBoard]->getActiveColor() == 'b') think();
}

void MainWindow::EnginePlayWhite() {
   engineW = true;
   if(game[activeBoard]->getActiveColor() == 'w') think();
}

void MainWindow::think() {
    /* Create Command with Fen-String */
    string command;
    command = "position fen " + game[activeBoard]->board->getFenstring();
    command.append("\ngo infinite");
    cout << command << endl;

    scores.clear();
    moves.clear();
    bestmove.clear();
    moved = false;
    multipvs.clear();
    engine.writeToEngine(command); //writeToStockfish(command);
    //engineView->show();
}

void MainWindow::readInput() {
    /*engine.writeToEngine(input->text().toStdString());
    return;*/
    if(chessserver) sendInputToServer(); else {
        /* Make a move */
        game[activeBoard]->move(input->text().toStdString());
        updateStatusBar();

        /* Aktivate oppent (chess engine) */
        if((engineB && game[activeBoard]->getActiveColor() == 'b') || (engineW && game[activeBoard]->getActiveColor() == 'w')) think();
    }
    input->clear();
}

/* Internet Chess Server Slots                                                */
/* -------------------------------------------------------------------------- */

bool MainWindow::ICSconnect() {
    chessserver = true;
    localboard = false;
    fics.connect();
    output->setParent(0);
    output->clear();
    output->setFixedSize(400, 300);
    output->show();
    icgamelist = new ICGameList();
    QObject::connect(icgamelist->list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onICGameListItemclicked(QListWidgetItem*)));
    //QObject::connect(icgamelist, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onICGameListItemclicked(QListWidgetItem*)));
    //output->setText(QString::fromStdString(fics.readSocket()));
}

void MainWindow::sendInputToServer() {
    //readICServer();
    output->setText(output->toPlainText().append(input->text()));
    output->verticalScrollBar()->setValue(output->verticalScrollBar()->maximum());
    sendToServer(input->text().toStdString());
}

void MainWindow::sendToServer(string msg) {
    cout << "sending to server: '" << msg << "'" << endl;
    size_t o = msg.find("observe ");
    size_t ex = msg.find("examine ");
    //cout << "o: " << o << endl;
    if(o != string::npos) {
        size_t sp = msg.find(" ", o + 8);
        cout << "sp: " << sp << endl;
        cout << msg.substr(o + 8, sp - o - 8) << endl;
        int gameID = boost::lexical_cast<int>(msg.substr(o + 8, sp - o - 8));
        game[activeBoard]->setGameID(gameID);
        BoardTab->setTabText(activeBoard, QString::fromStdString("#" + boost::lexical_cast<string>(gameID)));
        cout << "set gameID to " << msg.substr(o + 8, sp - o - 8) << endl;
    }
    if(ex != string::npos) {
        size_t sp = msg.find(" ", ex + 8);
        size_t sp2 = msg.find(" ", sp + 1);
        sp++;
        cout << "sp: " << sp2 << endl;
        cout << msg.substr(sp, sp2 - sp) << endl;
        int gameID = boost::lexical_cast<int>(msg.substr(sp, sp2));
        game[activeBoard]->setGameID(gameID); // will not work, cause this is not the real game id (it's the ID from history)
        BoardTab->setTabText(activeBoard, QString::fromStdString("#" + boost::lexical_cast<string>(gameID)));
        cout << "set gameID to " << msg.substr(sp, sp2) << endl;
    }
    fics.writeSocket(msg);//.append("\n"));
    //fics.writeSocket("help\n");
}

void MainWindow::setGameID() {
    bool ok;
    int gameID = QInputDialog::getInt(
                this, "Set Game ID", "Enter a new Game ID", 10, 0 , 999, 1, &ok);
    if(ok) game[activeBoard]->setGameID(gameID);

}

void MainWindow::setActiveColor() {
    bool ok;
    int activeColor = QInputDialog::getInt(
                this, "Set active color", "Enter a new color", 0, 0 , 1, 1, &ok);
    if(ok) {
        if(activeColor == 1) game[activeBoard]->setActiveColor('b');
        else game[activeBoard]->setActiveColor('w');
    }
}

void MainWindow::readICServer() {
    string line = fics.readFromServer();
    parseICSOutput(line);
    outstr += line;
    if(!fics.isunread()) {
        output->setText(QString::fromStdString(outstr));
        output->verticalScrollBar()->setValue(output->verticalScrollBar()->maximum());
    }
    /*while(chessserver) {
       string outstr = fics.readFromServer();
       while(outstr.size() > 0) {
           //parseICSOutput(outstr);
           cout << outstr;
           outstr = fics.readFromServer();
        }
       usleep(1000);
    }*/
}

void MainWindow::parseICSOutput(string outstr) {
    //string outstr = fics.readFromServer();
    string position;
    vector<string> fenstrings(13);

    size_t g = outstr.find("games");
    //boost::regex expr("[0-9]+[ ][0-9+-]+[ ]");
    //boost::regex expr("[0-9]+[ ][0-9+-]+[ ][A-z]+[ ][0-9+-]+[ ][A-z]+");
    boost::regex expr("[0-9]+[ ][0-9+-]+[ ][A-z]+[ ]+[0-9+-]+[ ][A-z]+[ ]+[\[][ ]?");

    boost::smatch m;
    if(boost::regex_search(outstr, m, expr)) {
        //cout << "REGEX MATCH" << endl;
        getICGameList = true;
        /*size_t gEnd = outstr.find("games displayed.");
        if(gEnd != string::npos) {
            getICGameList = false;
            icgamelist->show();
        } else {*/
            item.push_back(new QListWidgetItem);
            //cout << outstr << "|";
            item[item.size() - 1]->setText(QString::fromStdString(outstr.substr(0, outstr.size() - 1)));
            icgamelist->addItem(item[item.size() - 1]);
            //icgamelist->show();
            /*size_t newline = outstr.find("\n");
            size_t begin = g;
            size_t end = outstr.find("games displayed.");

            int i = 0;
            while(newline < end) {
                item.push_back(new QListWidgetItem);
                cout << outstr.substr(begin, newline - begin) << endl;
                item[i]->setText(QString::fromStdString(outstr.substr(begin, newline - begin)));
                icgamelist->addItem(item[i]);
                begin = newline + 1;
                newline = outstr.find("\n", newline + 1);
                i++;
                if(i > 1000) break;
            }*/
            //ICSgameList();
        //}
    } else if(getICGameList) {
        cout << "game list end" << endl;
        icgamelist->show(); getICGameList = false;
    }


    /* Parse style12-string */
    if(outstr.find("<12>") != string::npos) {
        cout << "style12 found " << endl;
        position.append(outstr.substr(outstr.find("<12>")));
        for (int j = 0; j < 8; j++) {
            fenstrings[j] = position.substr((5 + 9 * j), 8);
        }


        size_t b = 0; size_t e;
        int x = 0;
        e = position.find(" ", b + 1);
        values.clear();

        /* Get values from style12-string */
        while(e != string::npos && x < 50) {
            values.push_back(position.substr(b + 1,e - (b+1)));
            x++;
            b = e;
            e = position.find(" ", b + 1);
        }

        //cout << "set active color " << endl;
        fenstrings[8] = values[9][0] + 32; // active color (--> to lower case)
        //cout << "set castling values" << endl;
        if(values[11] == "1") fenstrings[9] = "K"; else fenstrings[9] = "-";
        if(values[12] == "1") fenstrings[9] += "Q"; else fenstrings[9] += "-";
        if(values[13] == "1") fenstrings[9] += "k"; else fenstrings[9] += "-";
        if(values[14] == "1") fenstrings[9] += "q"; else fenstrings[9] += "-";
        //fenstrings[9] += " ";
        if(values[10] == "-1") fenstrings[10] += "-"; else fenstrings[10] += values[10];
        //fenstrings[9] += " ";
        //cout << "set other value " << endl;
        fenstrings[11] += values[15];
        //fenstrings[9] += " ";
        //cout << "set move number" << endl;
        fenstrings[12] += values[26];
        //cout << fenstrings[9] << endl;

        /*for(int i=0; i < values.size(); i++) {
            cout << values[i] << "; ";
        }
        cout << endl;
        */
        //cout << "gameID: " << values[16] << endl;
        //cout << fenstrings[0] << " " << fenstrings[1] << fenstrings[2] << endl;
        bool isSet = false;
        for(int i = 0; i < Game::getNrOfGames(); i++) {
            cout << game[i]->getGameID() << " " << values[16] << endl;
            if(game[i]->getGameID() == boost::lexical_cast<int>(values[16])) {
                cout << "set position" << endl;
                game[i]->board->setPosition(Fen(fenstrings));
                game[i]->setActiveColor(values[9][0] + 32); // Uppercase to lower case
                //game[i]->board->show();
                isSet = true;
                break;
            }
        }
        if(!isSet) {
            newGame();
            game[activeBoard]->setGameID(boost::lexical_cast<int>(values[16]));
            cout << "setting GameID to " << boost::lexical_cast<int>(values[16]) << endl;
            cout << "set position" << endl;
            game[activeBoard]->board->setPosition(Fen(fenstrings));
            game[activeBoard]->setActiveColor(values[9][0] + 32); // Uppercase to lower case
        }

        /* If the game id of the server message matches the game id of the active board... */
        /* Set time and player names */
        if(game[activeBoard]->getGameID() == boost::lexical_cast<int>(values[16])) {
            /* write data to playerinfo labels */
            for(int i = 0; i < 2; i++){
                players[activeBoard * 2 + i] = values[17 + i];
                t[activeBoard * 2 + i] = boost::lexical_cast<int>(values[24 + 1 - i]);

            }
            time[1]->setTime(boost::lexical_cast<int>(values[24]));
            time[0]->setTime(boost::lexical_cast<int>(values[25]));
            player[1]->setText(values[17]);
            player[0]->setText(values[18]);
            game[activeBoard]->board->show();
            if(thinkOnMove) think();
        }
        updateStatusBar();

    }

    //output->setText(output->toPlainText().append(QString::fromStdString(outstr)));
    //output->verticalScrollBar()->setValue(output->verticalScrollBar()->maximum());
}

/*string MyWindow::makeTime(int seconds) {
    int m;
    string timestr;
    m = seconds / 60;
    seconds = seconds - m * 60;
    if(m < 10) timestr.append("0");
    timestr.append(boost::lexical_cast<string>(m));
    timestr.append(":");
    if(seconds < 10) timestr.append("0");
    timestr.append(boost::lexical_cast<string>(seconds));
    return timestr;
}*/

void MainWindow::clocks() {
    /* Update time for every game every second */
    for(int j = 0; j < Game::getNrOfGames(); j++) {
        int i = 0;
        if(game[j]->getActiveColor() == 'w') i = 1;
        t[j * 2 + i]--;
        if(j == activeBoard) time[i]->setTime(t[j * 2 + i]);
        //cout << (j * 2 + i) << endl;
    }
    /*for(int i = 0; i < 2* Game::getNrOfGames(); i++) {
        cout << boost::lexical_cast<string>(t[i]) << " ";
    }
    cout << endl;*/
}

void MainWindow::updateBoard() {
    //posIDLabel->setText(QString::number(posIDs[posIndex]));
    game[activeBoard]->board->getPositionFromDBByID(posIDs[activeBoard][posIndex[activeBoard]]);
    //fenstring->setText(QString::fromStdString(board->getFenstring()));
    game[activeBoard]->board->show();
}

void MainWindow::newGame() {
    cout << "new game " << endl;
    game.push_back(new Game());
    game[Game::getNrOfGames() - 1]->board->getPositionFromDBByID(1);

    /* Connect Square-Signals to MainWindow-Slots */
    /*for(int j = 0; j < game[Game::getNrOfGames() - 1]->board->squares.size(); j++) {
        QObject::connect(game[Game::getNrOfGames() - 1]->board->squares[j], SIGNAL(clicked(int)), this, SLOT(SquareClicked(int)));
        QObject::connect(game[Game::getNrOfGames() - 1]->board->squares[j], SIGNAL(dropped(int, int)), this, SLOT(SquareDropped(int, int)));
    }*/
    game[Game::getNrOfGames() - 1]->board->show(); // Write position to squares (QLabels)
    game[Game::getNrOfGames() - 1]->board->setPlayer(0, "Player " + lexical_cast<string>(players.size()), 0);
    game[Game::getNrOfGames() - 1]->board->setPlayer(1, "Player " + lexical_cast<string>(players.size() + 1), 0);
    game[Game::getNrOfGames() - 1]->board->newGame();
    posIDs.push_back(vector<int> ());
    posIndex.push_back(0);
    BoardBox.push_back(new QGroupBox);
    BoardBox[BoardBox.size() - 1]->setFixedSize(576,576);
    BoardTab->addTab(BoardBox[BoardBox.size() - 1], "Game " + QString::fromStdString(boost::lexical_cast<string>(BoardBox.size())));
    BoardBox[BoardBox.size() - 1]->setLayout(game[BoardBox.size() - 1]->board->Grid);
    for(int i = 0; i < 2; i++) {
        players.push_back("Player " + boost::lexical_cast<string>(2 * (Game::getNrOfGames() - 1) + i));
        t.push_back(0);
    }
    BoardTab->setCurrentIndex(Game::getNrOfGames() - 1);
    setBoardActive(Game::getNrOfGames() - 1);
}

void MainWindow::SquareClicked(int id) {
    /*int y = id / 8 + 1;
    int x = id % 8;
    string cmd;
    cmd.push_back(static_cast<char>(x + 97));
    cmd.push_back(static_cast<char>(y + 48));
    cout << "Square " << cmd << " clicked " << endl;
    if(clickcmd) {
        QString txt = input->text();
        input->setText(txt.append(QString::fromStdString(cmd)));
        clickcmd = false;
        readInput();
    } else {
        input->setText(QString::fromStdString(cmd));
        clickcmd = true;
    }*/
}

void MainWindow::SquareDropped(int target, int source) {
    int y = source / 8 + 1;
    int x = source % 8;
    string cmd;
    cmd.push_back(static_cast<char>(x + 97));
    cmd.push_back(static_cast<char>(y + 48));
    y = target / 8 + 1;
    x = target % 8;
    cmd.push_back(static_cast<char>(x + 97));
    cmd.push_back(static_cast<char>(y + 48));
    //Board b; b.move(); b.sh
    cout << "SquareDropped: " << cmd << endl;
    //if(chessserver) game[activeBoard]->move(cmd);
    //input->setText(QString::fromStdString(cmd));
    //readInput();
    if(chessserver) {
        output->setText(output->toPlainText().append(QString::fromStdString(cmd)));
        output->verticalScrollBar()->setValue(output->verticalScrollBar()->maximum());
        sendToServer(cmd);
        game[activeBoard]->move(cmd);
        updateStatusBar();
    } else {
        /* Make a move */
        game[activeBoard]->move(cmd);
        updateStatusBar();
        //game[activeBoard]->showMoveHistory();
        /* Aktivate oppent (chess engine) */
        engineController->go();
        if((engineB && game[activeBoard]->getActiveColor() == 'b') || (engineW && game[activeBoard]->getActiveColor() == 'w')) think();
    }
}

void MainWindow::quit() {
    int ret = QMessageBox::warning(this, "Quit", "Do you really want to quit?", QMessageBox::No | QMessageBox::Yes);
    if(ret == QMessageBox::Yes) close();
}

void MainWindow::ICSgameList() {
    input->setText(QString::fromStdString("games"));
    sendToServer("games");
    cout << "sent to server games " << endl;
    //icgamelist->show();
}

void MainWindow::onICGameListItemclicked(QListWidgetItem* item) {
    string gamestr = item->text().toStdString();
    string idstr = gamestr.substr(0, 3);
    size_t space = idstr.find(" ");
    int i = 0;
    while(space != string::npos) {
        space = idstr.find(" ", space + 1);
        i++;
    }
    idstr.erase(0, i);
    cout <<  idstr << endl;
    input->setText(QString::fromStdString("observe " + idstr));
    sendInputToServer();
    int gameID = boost::lexical_cast<int>(idstr);
}

void MainWindow::undock() {
    //GameInfoLayout->removeWidget(gameinfo);
    gameinfo->setParent(0);
    gameinfo->show();
}

void MainWindow::scanICS() {
    fics.scanFics();
}

void MainWindow::showPositionTree() {
    posTree = new QTreeWidget;
    posTree->setColumnCount(1);
    posTree->setMinimumSize(350, 600);
    vector<QTreeWidgetItem *> item;
    vector<int> posIDs = myChessDB.getPosIDsByParent(1);

    /* Level 0 */
    item.push_back(new QTreeWidgetItem());
    item[0]->setText(0, "1"); // Initial Position
    posTree->insertTopLevelItem(0, item[0]);
    QObject::connect(posTree, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(itemExpanded(QTreeWidgetItem*)));
    QObject::connect(posTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(showPosition(QTreeWidgetItem*)));
    QTreeWidgetItem* initItem = item[0];

    /* Level 1 */
    for(int i = 0; i < posIDs.size(); i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(initItem);
        item->setText(0, QString::fromStdString(boost::lexical_cast<string>(posIDs[i])));
    }
    posTree->show();
}

void MainWindow::itemExpanded(QTreeWidgetItem* item) {
    for(int j = 0; j < item->childCount(); j++) {
        vector<int> posIDs = myChessDB.getPosIDsByParent(item->child(j)->text(0).toInt());
        for(int i = 0; i < posIDs.size(); i++) {
            QTreeWidgetItem* newitem = new QTreeWidgetItem(item->child(j));
            newitem->setText(0, QString::fromStdString(boost::lexical_cast<string>(posIDs[i])));
        }
    }
}

void MainWindow::showPosition(QTreeWidgetItem* item) {
    int posID = item->text(0).toInt();
    game[activeBoard]->board->getPositionFromDBByID(posID);
    game[activeBoard]->board->show();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        cout << "left mouse button pressed" << endl;
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        //mimeData->setText(commentEdit->toPlainText());
        drag->setMimeData(mimeData);
        //drag->setPixmap(iconPixmap);

        Qt::DropAction dropAction = drag->exec();
    }
}

void MainWindow::linkClicked(QUrl url) {
    cout << url.toString().toStdString() << " clicked" << endl;
    string moves = url.toString().toStdString();
    int i = 0;
    while(i + 4 <= moves.size()){
        string move = moves.substr(i, 4);
        game[activeBoard]->move(move);
        updateStatusBar();
        i += 5;
    }

}

void MainWindow::updateStatusBar() {
    if(game[activeBoard]->getActiveColor() == 'w')
        statusActiveColor->setText("White to move");
    else
        statusActiveColor->setText("Black to move");
    statusMoveNr->setText(QString::fromStdString("Move " + boost::lexical_cast<string>(game[activeBoard]->getCurrentMoveNr())));
}

void MainWindow::showBestmoveMessage() {
    if(engineB && game[activeBoard]->getActiveColor() == 'b') game[activeBoard]->move(engine.getBestmove());
    else QMessageBox::information(this, "Bestmove", "Bestmove: " + QString::fromStdString(engine.getBestmove()), QMessageBox::Ok);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int ret = QMessageBox::warning(this, "Quitting Chessboard", "There are unsaved games. Do you want to save them?", QMessageBox::Close | QMessageBox::Cancel | QMessageBox::Save);
    if (ret == QMessageBox::Save) {
        game[activeBoard]->saveGame();
        //engine.exit();
        event->accept();
    } else if (ret == QMessageBox::Close) {
        //engine.exit();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::setPlayerName(int playerID) {
    bool ok;
    int color = 1 - playerID % 2;
    QString txt = QInputDialog::getText(this, "Renaming " + player[color]->getName(), "Enter a new name", QLineEdit::Normal, player[color]->getName(), &ok);
    if(ok && !txt.isEmpty()) {
        player[color]->setText(txt);
        players[playerID] = txt.toStdString();
        game[activeBoard]->board->setPlayer(color, txt.toStdString(), 0);
    }
}

void MainWindow::setNextPosition(int i) {
    nextPosIndex = i;
}

void MainWindow::unsetEngine(int i) {
    if(i % 2 == 0) engineW = false;
    else engineB = false;
}
