/**
 * base from github QtAwesome https://github.com/gamecreature/QtAwesome/tree/main
 * you can find icon and icon name in https://fa6.dashgame.com/
 */

#ifndef QTAWESOME_H
#define QTAWESOME_H

#include "enum_name_index.h"
#include "QtAwesomeAnim.h"

#include <QIcon>
#include <QIconEngine>
#include <QPainter>
#include <QRect>
#include <QVariantMap>

//---------------------------------------------------------------------------------------
#define QAwesome QtAwesome::instance()

class QtAwesomeIconPainter;
class QtAwesome;

/// The main class for managing icons
/// This class requires a 2-phase construction. You must first create the class and then initialize it via an init* method
class QtAwesome : public QObject
{
    Q_OBJECT

public:

    static QtAwesome *instance();
    virtual ~QtAwesome();

    void init( const QString& fontname );
    bool initFontAwesome();

    void addNamedCodepoint( const QString& name, int codePoint );
    QHash<QString, int> namedCodePoints() { return namedCodepoints_; }

    void setDefaultOption( const QString& name, const QVariant& value  );
    QVariant defaultOption( const QString& name );

    QIcon icon( int character, const QVariantMap& options = QVariantMap() );
    QIcon icon( int character, QColor color);
    QIcon icon( const QString& name, const QVariantMap& options = QVariantMap() );
    QIcon icon( const QString& name, QColor color);
    QIcon icon(QtAwesomeIconPainter* painter, const QVariantMap& optionMap = QVariantMap() );

    void give( const QString& name, QtAwesomeIconPainter* painter );

    QFont font(int character, int size );

    /// Returns the font-name that is used as icon-map
    void selectFont(int character);
    QString fontName() { return fontName_ ; }

private:
    static QScopedPointer<QtAwesome> m_instance;
    explicit QtAwesome(QObject *parent = QTAWESOME_NULL);

    QString fontName_;
    QString fontName_regular;
    QString fontName_brands;
    ///< The font name used for this map
    QHash<QString, int> namedCodepoints_;                  ///< A map with names mapped to code-points
    QList<int> brandsIds;

    QHash<QString, QtAwesomeIconPainter*> painterMap_;     ///< A map of custom painters
    QVariantMap defaultOptions_;                           ///< The default icon options
    QtAwesomeIconPainter* fontIconPainter_;                ///< A special painter fo painting codepoints
};


//---------------------------------------------------------------------------------------


/// The QtAwesomeIconPainter is a specialized painter for painting icons
/// your can implement an iconpainter to create custom font-icon code
class QtAwesomeIconPainter
{
public:
    virtual ~QtAwesomeIconPainter() {}
    virtual void paint( QtAwesome* awesome, QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state, const QVariantMap& options ) = 0;
};

Q_DECLARE_METATYPE(QtAwesomeAnimation*)

#endif // QTAWESOME_H
