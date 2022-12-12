//////////////////////////////////////////////////////////////////////////////
// oxygenanimations.cpp
// container for all animation engines
// -------------------
//
// SPDX-FileCopyrightText: 2006, 2007 Riccardo Iaconelli <riccardo@kde.org>
// SPDX-FileCopyrightText: 2006, 2007 Casper Boemann <cbr@boemann.dk>
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenanimations.h"
#include "oxygenpropertynames.h"
#include "oxygenstyleconfigdata.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDial>
#include <QGroupBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QProgressBar>
#include <QScrollBar>
#include <QSpinBox>
#include <QSplitterHandle>
#include <QTextEdit>
#include <QToolBar>
#include <QToolBox>
#include <QToolButton>

namespace Oxygen
{

//____________________________________________________________
Animations::Animations(QObject *parent)
    : QObject(parent)
{
    _widgetEnableStateEngine = new WidgetStateEngine(this);
    _spinBoxEngine = new SpinBoxEngine(this);
    _comboBoxEngine = new WidgetStateEngine(this);
    _toolButtonEngine = new WidgetStateEngine(this);
    _toolBoxEngine = new ToolBoxEngine(this);
    _busyIndicatorEngine = new BusyIndicatorEngine(this);

    registerEngine(_dockSeparatorEngine = new DockSeparatorEngine(this));
    registerEngine(_headerViewEngine = new HeaderViewEngine(this));
    registerEngine(_widgetStateEngine = new WidgetStateEngine(this));
    registerEngine(_progressBarEngine = new ProgressBarEngine(this));
    registerEngine(_menuBarEngine = new MenuBarEngineV1(this));
    registerEngine(_menuEngine = new MenuEngineV1(this));
    registerEngine(_scrollBarEngine = new ScrollBarEngine(this));
    registerEngine(_inputWidgetEngine = new WidgetStateEngine(this));
    registerEngine(_splitterEngine = new SplitterEngine(this));
    registerEngine(_tabBarEngine = new TabBarEngine(this));
    registerEngine(_toolBarEngine = new ToolBarEngine(this));
    registerEngine(_mdiWindowEngine = new MdiWindowEngine(this));
}

//____________________________________________________________
void Animations::setupEngines(void)
{
    // animation steps
    AnimationData::setSteps(StyleConfigData::animationSteps());

    {
        // default enability, duration and maxFrame
        bool animationsEnabled(StyleConfigData::animationsEnabled());

        // enability
        _widgetEnableStateEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _widgetStateEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _inputWidgetEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _comboBoxEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _toolButtonEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _toolBoxEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _splitterEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _scrollBarEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _spinBoxEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _tabBarEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _dockSeparatorEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _headerViewEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _mdiWindowEngine->setEnabled(animationsEnabled && StyleConfigData::genericAnimationsEnabled());
        _progressBarEngine->setEnabled(animationsEnabled && StyleConfigData::progressBarAnimationsEnabled());

        // busy indicator
        _busyIndicatorEngine->setEnabled(StyleConfigData::progressBarAnimated());

        // menubar engine
        int menuBarAnimationType(StyleConfigData::menuBarAnimationType());
        if (menuBarAnimationType == StyleConfigData::MB_FADE && !qobject_cast<MenuBarEngineV1 *>(_menuBarEngine)) {
            if (_menuBarEngine) {
                MenuBarEngineV1 *newEngine = new MenuBarEngineV1(this, _menuBarEngine);
                registerEngine(newEngine);
                _menuBarEngine->deleteLater();
                _menuBarEngine = newEngine;

            } else
                registerEngine(_menuBarEngine = new MenuBarEngineV1(this));

        } else if (menuBarAnimationType == StyleConfigData::MB_FOLLOW_MOUSE && !qobject_cast<MenuBarEngineV2 *>(_menuBarEngine)) {
            if (_menuBarEngine) {
                MenuBarEngineV2 *newEngine = new MenuBarEngineV2(this, _menuBarEngine);
                registerEngine(newEngine);
                _menuBarEngine->deleteLater();
                _menuBarEngine = newEngine;

            } else
                registerEngine(_menuBarEngine = new MenuBarEngineV1(this));
        }

        // menu engine
        int menuAnimationType(StyleConfigData::menuAnimationType());
        if (menuAnimationType == StyleConfigData::ME_FADE && !qobject_cast<MenuEngineV1 *>(_menuEngine)) {
            if (_menuEngine) {
                MenuEngineV1 *newEngine = new MenuEngineV1(this, _menuEngine);
                registerEngine(newEngine);
                _menuEngine->deleteLater();
                _menuEngine = newEngine;

            } else
                registerEngine(_menuEngine = new MenuEngineV1(this));

        } else if (menuAnimationType == StyleConfigData::ME_FOLLOW_MOUSE && !qobject_cast<MenuEngineV2 *>(_menuEngine)) {
            if (_menuEngine) {
                MenuEngineV2 *newEngine = new MenuEngineV2(this, _menuEngine);
                registerEngine(newEngine);
                _menuEngine->deleteLater();
                _menuEngine = newEngine;

            } else
                registerEngine(_menuEngine = new MenuEngineV1(this));
        }

        _menuBarEngine->setEnabled(animationsEnabled && menuBarAnimationType != StyleConfigData::MB_NONE);
        _menuEngine->setEnabled(animationsEnabled && menuAnimationType != StyleConfigData::ME_NONE);

        // toolbar engine
        int toolBarAnimationType(StyleConfigData::toolBarAnimationType());
        if (toolBarAnimationType == StyleConfigData::TB_NONE || toolBarAnimationType == StyleConfigData::TB_FOLLOW_MOUSE) {
            // disable toolbar engine
            _toolBarEngine->setEnabled(animationsEnabled && toolBarAnimationType == StyleConfigData::TB_FOLLOW_MOUSE);

            // unregister all toolbuttons that belong to a toolbar
            const auto widgets = _widgetStateEngine->registeredWidgets(AnimationHover | AnimationFocus);
            for (QWidget *widget : widgets) {
                if (qobject_cast<QToolButton *>(widget) && qobject_cast<QToolBar *>(widget->parentWidget())) {
                    _widgetStateEngine->unregisterWidget(widget);
                }
            }

        } else if (toolBarAnimationType == StyleConfigData::TB_FADE) {
            // disable toolbar engine
            _toolBarEngine->setEnabled(false);

            // retrieve all registered toolbars
            const BaseEngine::WidgetList widgets(_toolBarEngine->registeredWidgets());
            for (QWidget *widget : widgets) {
                // get all toolbuttons
                const auto children = widget->children();
                for (QObject *child : children) {
                    if (QToolButton *toolButton = qobject_cast<QToolButton *>(child)) {
                        _widgetStateEngine->registerWidget(toolButton, AnimationHover);
                    }
                }
            }
        }
    }

    {
        // durations
        _widgetEnableStateEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _widgetStateEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _inputWidgetEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _comboBoxEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _toolButtonEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _toolBoxEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _splitterEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _scrollBarEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _spinBoxEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _tabBarEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _dockSeparatorEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _headerViewEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _mdiWindowEngine->setDuration(StyleConfigData::genericAnimationsDuration());

        _progressBarEngine->setDuration(StyleConfigData::progressBarAnimationsDuration());
        _busyIndicatorEngine->setDuration(StyleConfigData::progressBarBusyStepDuration());

        _toolBarEngine->setDuration(StyleConfigData::genericAnimationsDuration());
        _toolBarEngine->setFollowMouseDuration(StyleConfigData::toolBarAnimationsDuration());

        _menuBarEngine->setDuration(StyleConfigData::menuBarAnimationsDuration());
        _menuBarEngine->setFollowMouseDuration(StyleConfigData::menuBarFollowMouseAnimationsDuration());

        _menuEngine->setDuration(StyleConfigData::menuAnimationsDuration());
        _menuEngine->setFollowMouseDuration(StyleConfigData::menuFollowMouseAnimationsDuration());
    }
}

//____________________________________________________________
void Animations::registerWidget(QWidget *widget) const
{
    if (!widget)
        return;

    // check against noAnimations propery
    QVariant propertyValue(widget->property(PropertyNames::noAnimations));
    if (propertyValue.isValid() && propertyValue.toBool())
        return;

    // these are needed to not register animations for kwin widgets
    if (widget->objectName() == QStringLiteral("decoration widget"))
        return;
    if (widget->inherits("KCommonDecorationButton"))
        return;
    if (widget->inherits("QShapedPixmapWidget"))
        return;

    // all widgets are registered to the enability engine.
    _widgetEnableStateEngine->registerWidget(widget, AnimationEnable);

    // install animation timers
    // for optimization, one should put with most used widgets here first
    if (qobject_cast<QToolButton *>(widget)) {
        _toolButtonEngine->registerWidget(widget, AnimationHover);
        bool isInToolBar(qobject_cast<QToolBar *>(widget->parent()));
        if (isInToolBar) {
            if (StyleConfigData::toolBarAnimationType() == StyleConfigData::TB_FADE) {
                _widgetStateEngine->registerWidget(widget, AnimationHover);
            }

        } else
            _widgetStateEngine->registerWidget(widget, AnimationHover | AnimationFocus);

    } else if (qobject_cast<QAbstractButton *>(widget)) {
        if (qobject_cast<QToolBox *>(widget->parent())) {
            _toolBoxEngine->registerWidget(widget);
        }

        _widgetStateEngine->registerWidget(widget, AnimationHover | AnimationFocus);

    } else if (qobject_cast<QDial *>(widget)) {
        _widgetStateEngine->registerWidget(widget, AnimationHover | AnimationFocus);

    }

    // groupboxes
    else if (QGroupBox *groupBox = qobject_cast<QGroupBox *>(widget)) {
        if (groupBox->isCheckable()) {
            _widgetStateEngine->registerWidget(widget, AnimationHover | AnimationFocus);
        }
    }

    // scrollbar
    else if (qobject_cast<QScrollBar *>(widget)) {
        _scrollBarEngine->registerWidget(widget);
    } else if (qobject_cast<QSlider *>(widget)) {
        _widgetStateEngine->registerWidget(widget, AnimationHover | AnimationFocus);
    } else if (qobject_cast<QProgressBar *>(widget)) {
        _progressBarEngine->registerWidget(widget);
        _busyIndicatorEngine->registerWidget(widget);
    } else if (qobject_cast<QSplitterHandle *>(widget)) {
        _splitterEngine->registerWidget(widget);
    } else if (qobject_cast<QMainWindow *>(widget)) {
        _dockSeparatorEngine->registerWidget(widget);
    } else if (qobject_cast<QHeaderView *>(widget)) {
        _headerViewEngine->registerWidget(widget);
    }

    // menu
    else if (qobject_cast<QMenu *>(widget)) {
        _menuEngine->registerWidget(widget);
    } else if (qobject_cast<QMenuBar *>(widget)) {
        _menuBarEngine->registerWidget(widget);
    } else if (qobject_cast<QTabBar *>(widget)) {
        _tabBarEngine->registerWidget(widget);
    } else if (qobject_cast<QToolBar *>(widget)) {
        _toolBarEngine->registerWidget(widget);
    }

    // editors
    else if (qobject_cast<QComboBox *>(widget)) {
        _comboBoxEngine->registerWidget(widget, AnimationHover);
        _inputWidgetEngine->registerWidget(widget, AnimationHover | AnimationFocus);
    } else if (qobject_cast<QSpinBox *>(widget)) {
        _spinBoxEngine->registerWidget(widget);
        _inputWidgetEngine->registerWidget(widget, AnimationHover | AnimationFocus);
    } else if (qobject_cast<QLineEdit *>(widget)) {
        _inputWidgetEngine->registerWidget(widget, AnimationHover | AnimationFocus);
    } else if (qobject_cast<QTextEdit *>(widget)) {
        _inputWidgetEngine->registerWidget(widget, AnimationHover | AnimationFocus);
    } else if (widget->inherits("KTextEditor::View")) {
        _inputWidgetEngine->registerWidget(widget, AnimationHover | AnimationFocus);
    }

    // lists
    else if (qobject_cast<QAbstractItemView *>(widget)) {
        _inputWidgetEngine->registerWidget(widget, AnimationHover | AnimationFocus);
    }

    // scrollarea
    else if (QAbstractScrollArea *scrollArea = qobject_cast<QAbstractScrollArea *>(widget)) {
        if (scrollArea->frameShadow() == QFrame::Sunken && (widget->focusPolicy() & Qt::StrongFocus)) {
            _inputWidgetEngine->registerWidget(widget, AnimationHover | AnimationFocus);
        }

    }

    // mdi subwindows
    else if (qobject_cast<QMdiSubWindow *>(widget)) {
        _mdiWindowEngine->registerWidget(widget);
    }

    return;
}

//____________________________________________________________
void Animations::unregisterWidget(QWidget *widget) const
{
    if (!widget)
        return;

    /*
    these are the engines that have not been stored
    inside the list, because they can be register widgets in combination
    with other engines
    */
    _widgetEnableStateEngine->unregisterWidget(widget);
    _spinBoxEngine->unregisterWidget(widget);
    _comboBoxEngine->unregisterWidget(widget);
    _toolButtonEngine->unregisterWidget(widget);
    _toolBoxEngine->unregisterWidget(widget);
    _busyIndicatorEngine->unregisterWidget(widget);

    // the following allows some optimization of widget unregistration
    // it assumes that a widget can be registered atmost in one of the
    // engines stored in the list.
    for (const BaseEngine::Pointer &engine : std::as_const(_engines)) {
        if (engine && engine.data()->unregisterWidget(widget))
            break;
    }
}

//_______________________________________________________________
void Animations::unregisterEngine(QObject *object)
{
    int index(_engines.indexOf(qobject_cast<BaseEngine *>(object)));
    if (index >= 0)
        _engines.removeAt(index);
}

//_______________________________________________________________
void Animations::registerEngine(BaseEngine *engine)
{
    _engines.append(engine);
    connect(engine, SIGNAL(destroyed(QObject *)), this, SLOT(unregisterEngine(QObject *)));
}
}
