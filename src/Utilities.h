#ifndef UTILITIES_H
#define UTILITIES_H

#include <QFont>

class Utilities
{
public:
    static QFont getMonospaceFont();

    static QSize scaleWithPrimaryScreenScalingFactor(QSize size);
};

#endif // UTILITIES_H
