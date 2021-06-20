//
// Created by corey on 3/20/21.
//

#ifndef CLIENTCHATAPP_LISTVIEWDELEGATE_H
#define CLIENTCHATAPP_LISTVIEWDELEGATE_H

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QTextDocument>
#include <qabstracttextdocumentlayout.h>
#include <QPainterPath>
#include <QBuffer>

class ListViewDelegate : public QAbstractItemDelegate
{
    int d_radius;
    int d_toppadding;
    int d_bottompadding;
    int d_leftpadding;
    int d_rightpadding;
    int d_verticalmargin;
    int d_horizontalmargin;
    int d_pointerwidth;
    int d_pointerheight;
    float d_widthfraction;
public:
    inline explicit ListViewDelegate(QObject *parent = nullptr);
protected:
    inline void paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const override;
    inline QSize sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const override;
};

inline ListViewDelegate::ListViewDelegate(QObject *parent)
        :
        QAbstractItemDelegate(parent),
        d_radius(5),
        d_toppadding(5),
        d_bottompadding(3),
        d_leftpadding(5),
        d_rightpadding(5),
        d_verticalmargin(15),
        d_horizontalmargin(10),
        d_pointerwidth(10),
        d_pointerheight(17),
        d_widthfraction(.7)
{}

inline void ListViewDelegate::paint(QPainter *painter, QStyleOptionViewItem const &option, QModelIndex const &index) const
{

    if (index.data(Qt::UserRole + 1).toString().contains("Image")) {

        painter->save();

        // load a resize base64 image for the view
        QByteArray original_size_base64;
        original_size_base64.append(index.data(Qt::DisplayRole).toString());
        QImage image;
        image.loadFromData(QByteArray::fromBase64(original_size_base64));

        QImage img_scaled = image.scaled(250,250, Qt::KeepAspectRatio);
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        img_scaled.save(&buffer, "PNG");
        QString base_64 = QString::fromLatin1(byteArray.toBase64().data());

        QTextDocument doc;
        doc.setHtml("<div><img src=\"data:image/png;base64," + base_64 + "/></div>");
        //painter->drawRect(option.rect);
        painter->translate(option.rect.left() + d_horizontalmargin,
                           option.rect.top() + ((index.row() == 0) ? d_verticalmargin : 0));

        QAbstractTextDocumentLayout::PaintContext ctx;
        // img_scaled.loadFromData(QByteArray::fromBase64(index.data(Qt::DisplayRole).toByteArray()));
        ctx.clip = QRectF( 0, 0, img_scaled.width(), img_scaled.height());
        doc.documentLayout()->draw(painter, ctx);

        painter->restore();

    }

    else {

        QTextDocument bodydoc;
        QTextOption textOption(bodydoc.defaultTextOption());
        textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        bodydoc.setDefaultTextOption(textOption);
        bodydoc.setDefaultFont(QFont("Roboto", 12));
        QString bodytext(index.data(Qt::DisplayRole).toString());
        bodydoc.setHtml(bodytext);

        qreal contentswidth =
                option.rect.width() * d_widthfraction - d_horizontalmargin - d_pointerwidth - d_leftpadding -
                d_rightpadding;
        bodydoc.setTextWidth(contentswidth);
        qreal bodyheight = bodydoc.size().height();

        bool outgoing = index.data(Qt::UserRole + 1).toString() == "Outgoing";

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        // uncomment to see the area provided to paint this item
        //painter->drawRect(option.rect);

        painter->translate(option.rect.left() + d_horizontalmargin,
                           option.rect.top() + ((index.row() == 0) ? d_verticalmargin : 0));

        // background color for chat bubble
        QColor bgcolor("#D3D3D3");
        if (outgoing)
            bgcolor = "#147efb";

        // create chat bubble
        QPainterPath pointie;

        // left bottom
        pointie.moveTo(0, bodyheight + d_toppadding + d_bottompadding);

        // right bottom
        pointie.lineTo(0 + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding - d_radius,
                       bodyheight + d_toppadding + d_bottompadding);
        pointie.arcTo(0 + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding - 2 * d_radius,
                      bodyheight + d_toppadding + d_bottompadding - 2 * d_radius,
                      2 * d_radius, 2 * d_radius, 270, 90);

        // right top
        pointie.lineTo(0 + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding, 0 + d_radius);
        pointie.arcTo(0 + contentswidth + d_pointerwidth + d_leftpadding + d_rightpadding - 2 * d_radius, 0,
                      2 * d_radius, 2 * d_radius, 0, 90);

        // left top
        pointie.lineTo(0 + d_pointerwidth + d_radius, 0);
        pointie.arcTo(0 + d_pointerwidth, 0, 2 * d_radius, 2 * d_radius, 90, 90);

        // left bottom almost (here is the pointie)
        pointie.lineTo(0 + d_pointerwidth, bodyheight + d_toppadding + d_bottompadding - d_pointerheight);
        pointie.closeSubpath();

        // rotate bubble for outgoing messages
        if (outgoing) {
            painter->translate(
                    option.rect.width() - pointie.boundingRect().width() - d_horizontalmargin - d_pointerwidth, 0);
            painter->translate(pointie.boundingRect().center());
            painter->rotate(180);
            painter->translate(-pointie.boundingRect().center());
        }

        // now paint it!
        painter->setPen(QPen(bgcolor));
        painter->drawPath(pointie);
        painter->fillPath(pointie, QBrush(bgcolor));

        // rotate back or painter is going to paint the text rotated...
        if (outgoing) {
            painter->translate(pointie.boundingRect().center());
            painter->rotate(-180);
            painter->translate(-pointie.boundingRect().center());
        }

        // set text color used to draw message body
        QAbstractTextDocumentLayout::PaintContext ctx;
        if (outgoing)
            ctx.palette.setColor(QPalette::Text, QColor("black"));
        else
            ctx.palette.setColor(QPalette::Text, QColor("white"));

        if (option.state & QStyle::State_Selected)
            ctx.palette.setColor(QPalette::Text, option.palette.color(QPalette::Active, QPalette::HighlightedText));


        // draw body text
        painter->translate((outgoing ? 0 : d_pointerwidth) + d_leftpadding, 0);
        bodydoc.documentLayout()->draw(painter, ctx);

        painter->restore();
    }
}

inline QSize ListViewDelegate::sizeHint(QStyleOptionViewItem const &option, QModelIndex const &index) const
{

    QTextDocument bodydoc;
    QTextOption textOption(bodydoc.defaultTextOption());
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    bodydoc.setDefaultTextOption(textOption);

    if (index.data(Qt::UserRole + 1) == "Image") {

        // load a resize base64 image
        QByteArray original_size_base64;
        original_size_base64.append(index.data(Qt::DisplayRole).toString());
        QImage image;
        image.loadFromData(QByteArray::fromBase64(original_size_base64));

        QImage img_scaled = image.scaled(250,250, Qt::KeepAspectRatio);
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        img_scaled.save(&buffer, "PNG");
        QString base_64 = QString::fromLatin1(byteArray.toBase64().data());

        bodydoc.setHtml("<div><img src=\"data:image/png;base64," +
                                base_64 + "/></div>");

        // p.translate to the right position
        //QSize size(img.width() * .20, img.height() * .20);
        QSize size(bodydoc.idealWidth() + d_horizontalmargin + d_pointerwidth + d_leftpadding + d_rightpadding,
                   bodydoc.size().height() + d_bottompadding + d_toppadding + d_verticalmargin + 1);
        return size;
    }

    else {

        bodydoc.setDefaultFont(QFont("Roboto", 12));
        QString bodytext(index.data(Qt::DisplayRole).toString());
        bodydoc.setHtml(bodytext);

        // the width of the contents are the (a fraction of the window width) minus (margins + padding + width of the bubble's tail)
        qreal contentswidth =
                option.rect.width() * d_widthfraction - d_horizontalmargin - d_pointerwidth - d_leftpadding -
                d_rightpadding;

        // set this available width on the text document
        bodydoc.setTextWidth(contentswidth);

        QSize size(bodydoc.idealWidth() + d_horizontalmargin + d_pointerwidth + d_leftpadding + d_rightpadding,
                   bodydoc.size().height() + d_bottompadding + d_toppadding + d_verticalmargin +
                   1); // I dont remember why +1, haha, might not be necessary

        if (index.row() == 0) // have extra margin at top of first item
            size += QSize(0, d_verticalmargin);

        return size;
    }
}

#endif //CLIENTCHATAPP_LISTVIEWDELEGATE_H
