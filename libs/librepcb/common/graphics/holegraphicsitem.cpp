/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "holegraphicsitem.h"

#include "../graphics/graphicslayer.h"
#include "origincrossgraphicsitem.h"

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

HoleGraphicsItem::HoleGraphicsItem(Hole&                           hole,
                                   const IF_GraphicsLayerProvider& lp,
                                   QGraphicsItem* parent) noexcept
  : PrimitivePathGraphicsItem(parent),
    mHole(hole),
    mLayerProvider(lp),
    mOnEditedSlot(*this, &HoleGraphicsItem::holeEdited) {
  // calculate path

  setPosition(mHole.getPosition());
  setPath(getPath(mHole));
  setRotation(mHole.getRotation());
  setLineLayer(mLayerProvider.getLayer(GraphicsLayer::sBoardDrillsNpth));
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setZValue(5);

  // add origin cross
  mOriginCrossGraphicsItem.reset(new OriginCrossGraphicsItem(this));
  mOriginCrossGraphicsItem->setRotation(Angle::deg45());
  mOriginCrossGraphicsItem->setSize(positiveToUnsigned(mHole.getDiameter()) +
                                    UnsignedLength(500000));
  mOriginCrossGraphicsItem->setLayer(
      mLayerProvider.getLayer(GraphicsLayer::sTopReferences));

  // register to the text to get attribute updates
  mHole.onEdited.attach(mOnEditedSlot);
}

HoleGraphicsItem::~HoleGraphicsItem() noexcept {
}

/*******************************************************************************
 *  Inherited from QGraphicsItem
 ******************************************************************************/

QPainterPath HoleGraphicsItem::shape() const noexcept {
  return PrimitivePathGraphicsItem::shape() + mOriginCrossGraphicsItem->shape();
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

QPainterPath HoleGraphicsItem::getPath(const Hole& hole) noexcept {
  QPainterPath path;
  qreal        halflength = hole.getLength()->toPx() / 2;
  qreal        radius     = hole.getDiameter()->toPx() / 2;
  path.moveTo(halflength, -radius);
  path.lineTo(-halflength, -radius);
  path.arcTo(-halflength - radius, -radius, 2 * radius, 2 * radius, 90, 180);
  path.lineTo(halflength, radius);
  path.arcTo(halflength - radius, -radius, 2 * radius, 2 * radius, 270, 180);
  return path;
}

void HoleGraphicsItem::holeEdited(const Hole& hole,
                                  Hole::Event event) noexcept {
  switch (event) {
    case Hole::Event::PositionChanged:
      setPosition(hole.getPosition());
      break;
    case Hole::Event::DiameterChanged:
      setPath(getPath(mHole));
      mOriginCrossGraphicsItem->setSize(positiveToUnsigned(hole.getDiameter()) +
                                        UnsignedLength(500000));
      break;
    case Hole::Event::LengthChanged:
      setPath(getPath(mHole));
      break;
    case Hole::Event::RotationChanged:
      setRotation(mHole.getRotation());
      break;
    default:
      qWarning() << "Unhandled switch-case in HoleGraphicsItem::holeEdited()";
      break;
  }
}

QVariant HoleGraphicsItem::itemChange(GraphicsItemChange change,
                                      const QVariant&    value) noexcept {
  if (change == ItemSelectedChange && mOriginCrossGraphicsItem) {
    mOriginCrossGraphicsItem->setSelected(value.toBool());
  }
  return QGraphicsItem::itemChange(change, value);
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb
