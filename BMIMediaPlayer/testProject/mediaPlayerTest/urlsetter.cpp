#include "urlsetter.h"
#include "ui_urlsetter.h"

UrlSetter::UrlSetter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UrlSetter)
{
    ui->setupUi(this);
}

UrlSetter::~UrlSetter()
{
    delete ui;
}

void UrlSetter::on_ptn_Ok_clicked()
{
    if(!ui->lineEdit_URL->text().isEmpty())
        emit playUrl(ui->lineEdit_URL->text());
//    this->close();
}

void UrlSetter::on_ptn_Cancle_clicked()
{
    ui->lineEdit_URL->setText("");
    this->close();
}
