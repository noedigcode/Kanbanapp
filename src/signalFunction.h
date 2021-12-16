/* SignalFunction
 * A helper class to conveniently call a function on the Qt event queue,
 * as if it was a signal/slot.
 */

#ifndef SIGNALFUNCTION_H
#define SIGNALFUNCTION_H

#include <QObject>
#include <QTimer>

#include <functional>

class SignalFunction
{
public:
    static void call(QObject* parent, std::function<void(void)> func)
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        QMetaObject::invokeMethod(parent, func, Qt::QueuedConnection);
#else
        QTimer::singleShot(0, parent, func);
#endif
    }
};

#endif // SIGNALFUNCTION_H
