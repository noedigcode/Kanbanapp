#include "Utilities.h"

#include <QFontDatabase>
#include <QFontInfo>
#include <QGuiApplication>
#include <QScreen>


QFont Utilities::getMonospaceFont()
{
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    if (!QFontInfo(font).fixedPitch()) {
        // Try backup method
        QStringList families({"monospace", "consolas", "courier new", "courier"});
        foreach (QString family, families) {
            font.setFamily(family);
            if (QFontInfo(font).fixedPitch()) { break; }
        }
    }
    return font;
}

QSize Utilities::scaleWithPrimaryScreenScalingFactor(QSize size)
{
    static const qreal baselineDpi = 96.0;

    qreal scalingFactor = QGuiApplication::primaryScreen()->logicalDotsPerInch()
                          / baselineDpi;
    return size * scalingFactor;
}
