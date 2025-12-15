/*
    QMPlay2 is a video and audio player.
    Copyright (C) 2010-2024  Błażej Szczygieł

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>

#include <PlayClass.hpp>
#include <Updater.hpp>
class VideoDock;

class VideoWidget final : public QMainWindow
{
public:
    VideoWidget(QList<QPair<QString, QString>> &argument);
    ~VideoWidget();

    VideoDock *getVideoDock();
    void toggleFullScreen();
    void toggleVisibility();
    void toggleCompactView();
    void toggleAlwaysOnTop(bool checked);
private:
    VideoDock *videoDock;
    void hideDockWidgetsAndDisableFeatures();
    void restoreDockWidgetFeatures();
    void hideDocks();
    void doRestoreState(const QByteArray &data, bool doToggleCompactView = false);
    bool isCompactView, wasShow, fullScreen, seekSFocus, m_restoreWindowOnVideo = false;
    bool m_compactViewBeforeFullScreen = false;
    bool m_loaded = false;
    QByteArray dockWidgetState, fullScreenDockWidgetState;
    QRect savedGeo;
    inline bool isTrayVisible() const;

#ifdef UPDATES
    Updater updater;
#endif
};


