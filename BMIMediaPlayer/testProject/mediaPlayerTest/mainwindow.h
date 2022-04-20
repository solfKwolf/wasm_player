#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <play.h>
#include "comdelegate.h"
#include "audiorender.h"
#include "videorender.h"
#include "glyuvrender.h"
#include "urlsetter.h"
#include "fileiodevice.h"
#include <iostream>
#include <mutex>
#include <QTimer>
#include <QLabel>

namespace Ui {
class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_openFile_clicked();

    void play(QString fileName);
    void play();
    void setSpeed(float speed);
    void autoSpeed();

    void setFileName(std::string fileName);

    void on_pushButton_screen_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_playPause_clicked();

    void onSliderMoved();

    void onVolumeSliderMoved();

    void showContextMenu(QPoint pos);

    void on_pushButton_playList_clicked();

    void setAudioFormat(int &sampleRate, int &audioChannels , int &audioFormat);

    void setAudioData(const unsigned char* pAudioData, int nAudioSize);
    void setVideoForamat(int &nWidth, int &nHeight, int &nFormat);
    void createVideoFrameOver();
    unsigned char* getVideoBuffer();
    void setVolume(float &volume);
    void clearRenderer();

    void positionChange(int position);
    void openFinishType(int type);
    void playFinishSignal();
    void playStop();
    int64_t readData(unsigned char* buffer, int len);
    int64_t seekData(int64_t offset, int whence);

    void on_pushButton_speedUp_clicked();

    void on_pushButton_speedDown_clicked();

    void getCodeRate();

    void on_pushButton_hardware_clicked();

protected:
    void contextMenuEvent(QContextMenuEvent * event);
private:
    Ui::MainWindow *ui;

    COMDelegate m_ComDelegate;
    IUNknown* pPlayUnknow;
    AudioRender* m_pAudioRender;
    GLYUVRender* m_pVideoRender;
    UrlSetter* m_pUrlSetter;
    qint64 m_nDuration;
    int m_nWidth;
    int m_nHeight;
    std::mutex m_mutex;
    bool m_bFullScreen;
    int m_nCurrentPos;
    QLabel *m_pCoderateLabel;
    QTimer m_coderateTimer;
    bool m_needHardWard;
    FileIODevice *m_fileIODevice;
    float m_volume;

};

#endif // MAINWINDOW_H
