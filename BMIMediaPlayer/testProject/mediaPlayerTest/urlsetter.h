#ifndef URLSETTER_H
#define URLSETTER_H

#include <QWidget>

namespace Ui {
class UrlSetter;
}

class UrlSetter : public QWidget
{
    Q_OBJECT

public:
    explicit UrlSetter(QWidget *parent = 0);
    ~UrlSetter();

Q_SIGNALS:
    void playUrl(QString);
private slots:
    void on_ptn_Ok_clicked();

    void on_ptn_Cancle_clicked();

private:
    Ui::UrlSetter *ui;
};

#endif // URLSETTER_H
