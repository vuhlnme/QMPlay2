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

#include <QApplication>
#include <QToolBar>
#include <QFrame>
#include <QSlider>
#include <QToolButton>
#include <QStatusBar>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QShortcut>
#include <QFileDialog>
#include <QTreeWidget>
#include <QListWidget>
#include <QWindow>
#include <QScreen>
#include <QActionGroup>
#include <qevent.h>

/* QMPlay2 gui */
#include <Appearance.hpp>
#include <VideoDock.hpp>
#include <VideoWidget.hpp>
#include <Functions.hpp>

/* MainWidgetTmpStyle -  dock widget separator extent must be larger for touch screens */
class VideoWidgetTmpStyle final : public QCommonStyle
{
public:
    ~VideoWidgetTmpStyle() = default;

    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override
    {
        const int pM = QCommonStyle::pixelMetric(metric, option, widget);
        if (metric == QStyle::PM_DockWidgetSeparatorExtent)
            return pM * 5 / 2;
        return pM;
    }
};

/* VideoWidget */
VideoWidget::VideoWidget(QList<QPair<QString, QString>> &arguments)
#ifdef UPDATES
    : updater(this)
#endif
{
    /* Looking for touch screen */
    if (Functions::hasTouchScreen())
    {
        VideoWidgetTmpStyle mainWidgetTmpStyle;
        setStyle(&mainWidgetTmpStyle);
    }


    setObjectName("VideoWidget");

    setDockOptions(AllowNestedDocks | AnimatedDocks | AllowTabbedDocks);
    setMouseTracking(true);

    videoDock = new VideoDock;
    videoDock->setObjectName("videoDock");
    addDockWidget(Qt::LeftDockWidgetArea, videoDock);
    Appearance::init();

    showMaximized();
    setVisible(true);

}
VideoWidget::~VideoWidget()
{
}


void VideoWidget::hideDockWidgetsAndDisableFeatures()
{
    addDockWidget(Qt::RightDockWidgetArea, videoDock);
}
void VideoWidget::restoreDockWidgetFeatures()
{
}

void VideoWidget::toggleCompactView()
{
    if (!isCompactView)
    {
        dockWidgetState = saveState();

        hideDockWidgetsAndDisableFeatures();
        videoDock->show();

        videoDock->fullScreen(true);

        isCompactView = true;
    }
    else
    {
        videoDock->setLoseHeight(0);
        isCompactView = false;

        videoDock->fullScreen(false);

        restoreState(dockWidgetState);
        dockWidgetState.clear();

        restoreDockWidgetFeatures();
    }
}

void VideoWidget::toggleFullScreen()
{
    static bool visible, tb_movable;
#ifndef Q_OS_ANDROID
    static bool maximized;
#endif
#ifdef Q_OS_MACOS
    if (isFullScreen())
    {
        showNormal();
        return;
    }
#endif
    if (!fullScreen)
    {
        visible = isVisible();

        if ((m_compactViewBeforeFullScreen = isCompactView))
            toggleCompactView();

#ifndef Q_OS_ANDROID
        maximized = isMaximized();

#ifndef Q_OS_MACOS
#ifndef Q_OS_WIN
        if (isFullScreen())
#endif
            videoDock->showNormal();
#endif

        dockWidgetState = saveState();
        if (!maximized)
            savedGeo = geometry();
#else
        dockWidgetState = saveState();
#endif // Q_OS_ANDROID
        hide();

        hideDockWidgetsAndDisableFeatures();
        videoDock->fullScreen(true);
        showFullScreen();
        fullScreen = true;
        showFullScreen();
    }
    else
    {
        videoDock->setLoseHeight(0);
        fullScreen = false;

#ifndef Q_OS_ANDROID
        showNormal();
#endif // Q_OS_MACOS
        if (maximized)
            showMaximized();
        else
        {
            setGeometry(savedGeo);
        }

        showMaximized();
#ifdef Q_OS_WIN
        QCoreApplication::processEvents();
#endif

        videoDock->fullScreen(false);

        restoreState(dockWidgetState);
        dockWidgetState.clear();

        if (!visible) //jeżeli okno było wcześniej ukryte, to ma je znowu ukryć
            toggleVisibility();

        restoreDockWidgetFeatures();

        if (m_compactViewBeforeFullScreen)
            toggleCompactView();


        setFocus();
    }
}

VideoDock *VideoWidget::getVideoDock()
{
    return videoDock;
}

inline bool VideoWidget::isTrayVisible() const
{
    return false;
}

void VideoWidget::doRestoreState(const QByteArray &data, bool doToggleCompactView)
{
    if (isMaximized())
    {
        setUpdatesEnabled(false);
    }
    else
    {
        restoreState(data);
    }
}


void VideoWidget::toggleVisibility()
{
#ifndef Q_OS_ANDROID
    static bool maximized;
    if (isVisible())
    {
        if (fullScreen)
            toggleFullScreen();
        if (isMaximized())
        {
            if (!isCompactView)
                dockWidgetState = saveState();
            maximized = true;
            showNormal();
        }

        hide();
    }
    else
    {
        if (!maximized)
            showNormal();
        else
        {
            showMaximized();
            if (!isCompactView && !dockWidgetState.isEmpty())
            {
                doRestoreState(dockWidgetState);
                dockWidgetState.clear();
            }
            maximized = false;
        }
        activateWindow();
        raise();
    }
#endif
}
