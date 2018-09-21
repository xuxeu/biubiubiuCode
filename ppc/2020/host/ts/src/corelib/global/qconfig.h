// Everything

#ifndef QT_DLL
#define QT_DLL
#endif

/* License information */
#define QT_PRODUCT_LICENSEE "DenKor"
#define QT_PRODUCT_LICENSE "Universal"

// Qt Edition
#ifndef QT_EDITION
#  define QT_EDITION QT_EDITION_UNIVERSAL
#endif

/* Machine byte-order */
#define Q_BIG_ENDIAN 4321
#define Q_LITTLE_ENDIAN 1234
#define Q_BYTE_ORDER Q_LITTLE_ENDIAN

// Compile time features
#if defined(QT_NO_STYLE_WINDOWSXP) && defined(QT_STYLE_WINDOWSXP)
# undef QT_NO_STYLE_WINDOWSXP
#elif !defined(QT_NO_STYLE_WINDOWSXP)
# define QT_NO_STYLE_WINDOWSXP
#endif

