#ifndef QSQUARE_H
#define QSQUARE_H
#include <QLabel>

class QSquare : public QLabel {
    Q_OBJECT
public:
    QSquare();
    static QString PATH;
    void emitSignal();
    void setPixmap(QPixmap p);
    void highlight(bool);
    void highlight(bool, QString color);
    void clearHighlight(QString style);
private:
    static int ID;
    int id;
    QPoint dragStartPosition;
    QLabel* piece;
    QPixmap square_d = QPixmap(PATH + "textures/wood_d.png");
    QPixmap square_l = QPixmap(PATH + "textures/wood_l.png");
protected:
  void mousePressEvent(QMouseEvent * e);
  void mouseMoveEvent(QMouseEvent *event);
  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);
signals:
    void clicked (int);
    void dropped (int, int);
};

#endif // QSQUARE_H
