#ifndef RAKNETWORKER_H
#define RAKNETWORKER_H
#include <QtCore/QThread>
#include <QtGui/QStandardItemModel>
#include <cstdlib>

#include "common.h"
#include "globals.h"
#include "xmlsets.h"
#include "Game/PlayerPos.h"
#include "Game/playback.h"

#include "scorebwindow.h"

class RakNetWorker : public QThread {
    Q_OBJECT

public:
    RakNetWorker();
    ~RakNetWorker();

    void run() override;
};
#endif
