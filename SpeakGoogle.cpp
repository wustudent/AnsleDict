#include "SpeakGoogle.h"
#include <QTextCodec>
#include <QTimer>
#include "PathMgr.h"
#include "QDebug"
#include <QMutex>

SpeakGoogle::SpeakGoogle(QObject *parent) :
    QObject(parent)
{
    /////////////////////////////////////////////////////////////////
    //phonon
    mediaObject = new Phonon::MediaObject(this);
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(mediaObject, audioOutput);

    connect(mediaObject,
            SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this,
            SLOT(stateChanged(Phonon::State, Phonon::State)));
    time = 0;
}

SpeakGoogle::~SpeakGoogle()
{
    mediaObject->deleteLater();
    mediaObject = NULL;
    audioOutput->deleteLater();
    audioOutput = NULL;
}

void SpeakGoogle::stateChanged(Phonon::State newState,
                               Phonon::State oldstate)
{
    Q_UNUSED(oldstate);
    curState = newState;
    if(curState == Phonon::PausedState)
    {
        qDebug() << "状态改变，开始发音";
        startSpeak();
    }
    else if(curState == Phonon::StoppedState)
    {
        qDebug() << "状态改变，停止发音，结束信号发送";
        emit soundEnded();
    }
    else if(curState == Phonon::ErrorState)
    {
        qDebug() << "状态改变，发声错误，结束信号发送";

        emit speakError();
    }
}

void SpeakGoogle::startSpeak()
{
    qDebug() << "开始发音";
    mediaObject->setCurrentSource(Phonon::MediaSource(myurl));
    //开始发音
    googleTTS();
}

void SpeakGoogle::googleTTS()
{
    qDebug() << "开始发音，判断播放次数";
    speakGoogleRestart();
    time++;
    if(time > soundTime)
    {
        qDebug() << "播放次数达到，停止发音";
        googleTTSStop();

    }
    else
    {
        qDebug() << "播放次数未达到，继续发音";
        googleTTSPlay();
    }
}

void SpeakGoogle::setSoundTime(const int &soundtime)
{
    soundTime = soundtime;
}

void SpeakGoogle::speakGoogleTTS(const QString &text)
{
    if(mediaObject->currentTime() == mediaObject->totalTime())
    {
        mediaObject->seek(0);
    }
    time = 0;
    ///////////////////////////////////////////////////////////////////////////////////
    //
    wordText = text;
    QString strUrl = QString("http://translate.google.com/translate_tts?tl=de&q=%1")
                            .arg(wordText);
    QUrl url(strUrl); 
    myurl = url;
    startSpeak();
}

void SpeakGoogle::speakGoogleRestart()
{
    qDebug() << "重复发音重置播放位置";
    if(mediaObject->currentTime() == mediaObject->totalTime())
    {
        mediaObject->seek(0);
    }
}

void SpeakGoogle::googleTTSPlay()
{
    mediaObject->play();
}

void SpeakGoogle::googleTTSStop()
{
    mediaObject->stop();
//    mediaObject->clear();
}

Phonon::MediaObject *SpeakGoogle::getMediaObject()
{
    return mediaObject;
}

void SpeakGoogle::NetWorkIsBad()
{
    googleTTSStop();
}
