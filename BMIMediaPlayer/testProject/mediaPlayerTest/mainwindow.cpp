#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "fileiodevice.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QContextMenuEvent>
#include <QImage>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
#ifdef __WIN32__
    , m_ComDelegate("MediaPlayer.dll")
#else
    , m_ComDelegate("libMediaPlayer.so")
#endif
    , m_pAudioRender(new AudioRender)
    , m_pUrlSetter(new UrlSetter)
    , m_nDuration(-1)
    , m_bFullScreen(false)
    , m_nCurrentPos(0)
    , m_pCoderateLabel(new QLabel(this))
    , m_needHardWard(false)
    , m_fileIODevice(new FileIODevice)
    , m_volume(1)
{
    ui->setupUi(this);
    pPlayUnknow = m_ComDelegate.createUnknow();
    connect(m_pUrlSetter, SIGNAL(playUrl(QString)), this, SLOT(play(QString)));
    connect(ui->Slider, SIGNAL(sliderReleased()), this, SLOT(onSliderMoved()));
    connect(ui->Slider_Volume,SIGNAL(sliderReleased()),this,SLOT(onVolumeSliderMoved()));
    connect(&m_coderateTimer, SIGNAL(timeout()), this, SLOT(getCodeRate()));
    m_pCoderateLabel->raise();
    m_pCoderateLabel->setFont(QFont("Arial", 15));
    m_pCoderateLabel->resize(100, m_pCoderateLabel->height());
    m_pCoderateLabel->move(10,10);
    QPalette palette = m_pCoderateLabel->palette();
    palette.setColor(QPalette::WindowText, Qt::red);
    palette.setColor(QPalette::Background, QColor(0, 0, 0, 0));
    m_pCoderateLabel->setPalette(palette);
    m_pCoderateLabel->setText(QString::number(0) + "KB");
    m_pCoderateLabel->setVisible(true);
}

MainWindow::~MainWindow()
{
    pPlayUnknow->Release();
    delete ui;
    delete m_pAudioRender;
    delete m_pUrlSetter;
    delete m_pCoderateLabel;
}

void MainWindow::on_pushButton_openFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Please select a file."),QString(),tr("*"));
    if(fileName.isEmpty())
        return;

    play(fileName);
}

void MainWindow::play(QString fileName)
{
    setFileName(fileName.toStdString());
    play();
}

void MainWindow::play()
{
    if(!pPlayUnknow)
    {
        QMessageBox::information(this,tr("error"),tr("No play component loaded."));
        return;
    }
    // 查询播放器接口
    IPtr<IPlay,&IID_IPLAY> spIPlay(pPlayUnknow);
    if(spIPlay)
    {
        // 查询成功
        auto callback_openFinishType = std::bind(&MainWindow::openFinishType,this,std::placeholders::_1);
        auto callback_setAudio = std::bind(&MainWindow::setAudioFormat,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
        auto callback_getFrameBuffer = std::bind(&MainWindow::getVideoBuffer,this);
        auto callback_setAudioData = std::bind(&MainWindow::setAudioData,this,std::placeholders::_1,std::placeholders::_2);
        auto callback_getVideoFormat = std::bind(&MainWindow::setVideoForamat,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
        auto callback_createVideo = std::bind(&MainWindow::createVideoFrameOver,this);
        auto callback_clearRenderer = std::bind(&MainWindow::clearRenderer,this);
        auto callback_playFinish = std::bind(&MainWindow::playFinishSignal,this);
        auto callback_playStop = std::bind(&MainWindow::playStop,this);
        spIPlay->openFinishType(callback_openFinishType);
        auto callback_positionChange = std::bind(&MainWindow::positionChange,this,std::placeholders::_1);

        spIPlay->setAudioRenderFormat(callback_setAudio);
        spIPlay->setAudioRenderData(callback_setAudioData);
        spIPlay->setVideoRenderFormat(callback_getVideoFormat);
        spIPlay->getVideoFrameBuffer(callback_getFrameBuffer);
        spIPlay->createVideoFrameOver(callback_createVideo);
        spIPlay->clearRenderer(callback_clearRenderer);
        spIPlay->positionChange(callback_positionChange);
        spIPlay->playFinish(callback_playFinish);
        spIPlay->playStop(callback_playStop);
//        autoSpeed();
        spIPlay->setCacheTime(2);
//        spIPlay->setCodeRateSecond(1);

        if(ui->checkBox_UseIODevice->isChecked())
        {
            auto callback_ioRead = std::bind(&FileIODevice::readData, m_fileIODevice, std::placeholders::_1, std::placeholders::_2);
            auto callback_ioSeek = std::bind(&FileIODevice::seekData, m_fileIODevice, std::placeholders::_1, std::placeholders::_2);
            spIPlay->setIOCallBack(callback_ioRead);
            spIPlay->setSeekCallBack(callback_ioSeek);
            m_fileIODevice->close();
            m_fileIODevice->setFileName(spIPlay->GetFileName());
            if(m_fileIODevice->open(QIODevice::ReadOnly))
            {
                spIPlay->PlayStream(true);
            }
        }
        else
        {
            spIPlay->Play();
        }
        spIPlay->setQRCodeText("SmartData");

        getCodeRate();
    }
    else
    {
        // 查询接口失败
        QMessageBox::information(this,tr("error"),tr("Component hasn't got a play function."));
    }
}

void MainWindow::setSpeed(float speed)
{
    if(!pPlayUnknow)
    {
        QMessageBox::information(this,tr("error"),tr("No play component loaded."));
        return;
    }
    // 查询播放器接口
    IPtr<IPlay,&IID_IPLAY> spIPlay(pPlayUnknow);
    if(spIPlay)
    {
        // 查询成功
        spIPlay->setSpeed(speed);
    }
    else
    {
        // 查询接口失败
        QMessageBox::information(this,tr("error"),tr("Component hasn't got a play function."));
    }
}

void MainWindow::autoSpeed()
{
    if(!pPlayUnknow)
    {
        QMessageBox::information(this,tr("error"),tr("No play component loaded."));
        return;
    }
    // 查询播放器接口
    IPtr<IPlay,&IID_IPLAY> spIPlay(pPlayUnknow);
    if(spIPlay)
    {
        // 查询成功
        spIPlay->setMaxAutoSpeed(1.5);
        spIPlay->autoSpeed();
    }
    else
    {
        // 查询接口失败
        QMessageBox::information(this,tr("error"),tr("Component hasn't got a play function."));
    }
}

void MainWindow::setFileName(std::string fileName)
{
    if(!pPlayUnknow)
    {
        QMessageBox::information(this,tr("error"),tr("No play component loaded."));
        return;
    }

    // 查询播放器接口
    IPtr<IPlay,&IID_IPLAY> spIPlay(pPlayUnknow);
    if(spIPlay)
    {
        // 查询成功
        spIPlay->SetFileName(fileName.c_str());
    }
    else
    {
        // 查询接口失败
        QMessageBox::information(this,tr("error"),tr("Component hasn't got a play function."));
    }
}

void MainWindow::on_pushButton_screen_clicked()
{
    if(!m_bFullScreen)
    {
        m_bFullScreen = true;
        this->setWindowFlags(Qt::Window);
        this->showFullScreen();
    }
    else
    {
        m_bFullScreen = false;
        this->setWindowFlags(Qt::SubWindow);
        this->showNormal();
    }
}

void MainWindow::on_pushButton_stop_clicked()
{
    if(!pPlayUnknow)
    {
        QMessageBox::information(this,tr("error"),tr("No play component loaded."));
        return;
    }

    // 查询播放器接口
    IPtr<IPlay,&IID_IPLAY> spIPlay(pPlayUnknow);
    if(spIPlay)
    {
        // 查询成功
        auto callback_clearRenderer = std::bind(&MainWindow::clearRenderer,this);
        spIPlay->clearRenderer(callback_clearRenderer);
        spIPlay->Stop();
    }
    else
    {
        // 查询接口失败
        QMessageBox::information(this,tr("error"),tr("Component hasn't got a play function."));
    }
}

void MainWindow::on_pushButton_playPause_clicked()
{
    if(!pPlayUnknow)
    {
        QMessageBox::information(this,tr("error"),tr("No play component loaded."));
        return;
    }

    // 查询播放器接口
    IPtr<IPlay,&IID_IPLAY> spIPlay(pPlayUnknow);
    if(spIPlay)
    {
        // 查询成功
        spIPlay->TogglePause();
    }
    else
    {
        // 查询接口失败
        QMessageBox::information(this,tr("error"),tr("Component hasn't got a play function."));
    }
}

void MainWindow::onSliderMoved()
{
    int nValue = ui->Slider->sliderPosition();
    if(!pPlayUnknow)
    {
        QMessageBox::information(this,tr("error"),tr("No play component loaded."));
        return;
    }

    // 查询播放器接口
    IPtr<IPlay,&IID_IPLAY> spIPlay(pPlayUnknow);
    if(spIPlay)
    {
        // 查询成功
        int nDuration = spIPlay->Duration();
        spIPlay->Seek(nValue*nDuration / 1000);
    }
    else
    {
        // 查询接口失败
        QMessageBox::information(this,tr("error"),tr("Component hasn't got a play function."));
    }
}

void MainWindow::onVolumeSliderMoved()
{
    int nValue = ui->Slider_Volume->value();
    if(!pPlayUnknow)
    {
        QMessageBox::information(this,tr("error"),tr("No play component loaded."));
        return;
    }

    // 查询播放器接口
    IPtr<IPlay,&IID_IPLAY> spIPlay(pPlayUnknow);
    if(spIPlay)
    {
        // 查询成功
        float volume = (float)nValue/10;
        spIPlay->setVolume(volume);
        setVolume(volume);
    }
    else
    {
        // 查询接口失败
        QMessageBox::information(this,tr("error"),tr("Component hasn't got a play function."));
    }
}

void MainWindow::showContextMenu(QPoint pos)
{
    QMenu *pMenu = new QMenu(this);
    if(pMenu)
    {
        pMenu->addAction("打开URL");
    }
    QAction*pAction = pMenu->exec(pos);
    if(pAction)
    {
        if(pAction->text() == "打开URL")
        {
            m_pUrlSetter->show();
        }
    }
    delete pMenu;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    showContextMenu(event->globalPos());
    QMainWindow::contextMenuEvent(event);
}

void MainWindow::on_pushButton_playList_clicked()
{
    if(!pPlayUnknow)
    {
        QMessageBox::information(this,tr("error"),tr("No play component loaded."));
        return;
    }

    // 查询播放器接口
    IPtr<IPlay,&IID_IPLAY> spIPlay(pPlayUnknow);
    if(spIPlay)
    {
        // 查询成功
        float scale = spIPlay->getScale();
        if(scale >= 4.0)
            scale = 0.5;
        else
            scale *= 2;
        spIPlay->setScale(scale);
    }
    else
    {
        // 查询接口失败
        QMessageBox::information(this,tr("error"),tr("Component hasn't got a play function."));
    }
}

void MainWindow::setAudioFormat(int &sampleRate, int &audioChannels, int &audioFormat)
{
    if(!m_pAudioRender->isInit())
    {
        m_pAudioRender->setAudioFormat(sampleRate,audioChannels,audioFormat);
    }
}

void MainWindow::setAudioData(const unsigned char *pAudioData, int nAudioSize)
{
    m_pAudioRender->setData(QByteArray((const char*)pAudioData, nAudioSize));
}

void MainWindow::setVideoForamat(int &nWidth, int &nHeight, int &nFormat)
{
    ui->widget_screen->setVideoInfo(nWidth,nHeight,nFormat);
}

void MainWindow::createVideoFrameOver()
{
    ui->widget_screen->createVideoFrameOver();
}

unsigned char *MainWindow::getVideoBuffer()
{
    return ui->widget_screen->getVideoFrameBuffer();
}

void MainWindow::setVolume(float &volume)
{
    m_volume = volume;
    m_pAudioRender->setVolume(m_volume);
    qDebug()<<"volume is "<< volume;
}

void MainWindow::clearRenderer()
{
    if(m_pAudioRender)
    {
        m_pAudioRender->clearRenderer();
    }
    ui->widget_screen->clearRenderer();
}

void MainWindow::positionChange(int position)
{
    if(m_nCurrentPos!=position)
    {
        qDebug()<<" position : "<<position;
        m_nCurrentPos = position;
    }
}

void MainWindow::openFinishType(int type)
{
    if(type == 0)
    {
        qDebug()<<" openFinishType :  openError ";
    }
    else
    {
        qDebug()<<" openFinishType :  openSuccess ";
    }
}

void MainWindow::playFinishSignal()
{
    qDebug()<<" playFinish!!! ";
    m_fileIODevice->close();
}

void MainWindow::playStop()
{
    qDebug()<<" playStop!!! ";
}

int64_t MainWindow::readData(unsigned char *buffer, int len)
{
    return m_fileIODevice->readData(buffer,len);
}

int64_t MainWindow::seekData(int64_t offset, int whence)
{
    return m_fileIODevice->seekData(offset,whence);
}

void MainWindow::on_pushButton_speedUp_clicked()
{
    float speed = ui->label_speed->text().toFloat();
    if(speed>=1.0 && speed <4.0)
    {
        speed +=(float)0.1;
    }
    ui->label_speed->setText(QString::number(speed));
    setSpeed(speed);
}

void MainWindow::on_pushButton_speedDown_clicked()
{
    float speed = ui->label_speed->text().toFloat();
    if(speed >1.0 && speed <= 4.0)
    {
        speed -= (float)0.1;
    }
    ui->label_speed->setText(QString::number(speed));
    setSpeed(speed);
}

void MainWindow::getCodeRate()
{
    if(!pPlayUnknow)
    {
        QMessageBox::information(this,tr("error"),tr("No play component loaded."));
        return ;
    }

    // 查询播放器接口
    IPtr<IPlay,&IID_IPLAY> spIPlay(pPlayUnknow);
    if(spIPlay)
    {
        // 查询成功
        int codeRate = spIPlay->getCoderate();
        m_pCoderateLabel->setText(QString::number(codeRate/1024) + "KB");
        m_coderateTimer.start(1000);
        return;
    }
    else
    {
        // 查询接口失败
        QMessageBox::information(this,tr("error"),tr("Component hasn't got a play function."));
    }
}

void MainWindow::on_pushButton_hardware_clicked()
{
    if(!pPlayUnknow)
    {
        QMessageBox::information(this,tr("error"),tr("No play component loaded."));
        return ;
    }

    // 查询播放器接口
    IPtr<IPlay,&IID_IPLAY> spIPlay(pPlayUnknow);
    if(spIPlay)
    {
        // 查询成功
        m_needHardWard = !(spIPlay->getHardware());
        spIPlay->setHardware(m_needHardWard);
        qDebug()<<" needhardware : " << spIPlay->getHardware();
        return;
    }
    else
    {
        // 查询接口失败
        QMessageBox::information(this,tr("error"),tr("Component hasn't got a play function."));
    }
}
