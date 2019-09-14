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

#include <gtest/gtest.h>
#include <librepcb/common/geometry/pathmodel.h>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace tests {

/*******************************************************************************
 *  Test Class
 ******************************************************************************/

class PathModelTest : public ::testing::Test {
protected:
  static Path createPopulatedPath() noexcept {
    Path path;
    path.addVertex(Point(1, 2), Angle(3));
    path.addVertex(Point(0, 0), Angle(0));
    path.addVertex(Point(1000, 2000), Angle(3000));
    return path;
  }
};

/*******************************************************************************
 *  Test Methods
 ******************************************************************************/

TEST_F(PathModelTest, testSetDataRoundsNumbers) {
  PathModel model(nullptr);
  model.setPath(createPopulatedPath());
  bool ret = model.setData(model.index(1, PathModel::COLUMN_X), 5.08);
  EXPECT_TRUE(ret);
  ret = model.setData(model.index(1, PathModel::COLUMN_Y), 1.23456789);
  EXPECT_TRUE(ret);
  ret = model.setData(model.index(1, PathModel::COLUMN_ANGLE), 45.0000001);
  EXPECT_TRUE(ret);

  Path expected             = createPopulatedPath();
  expected.getVertices()[1] = Vertex(Point(5080000, 1234568), Angle(45000000));
  EXPECT_EQ(expected, model.getPath());
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace tests
}  // namespace librepcb
