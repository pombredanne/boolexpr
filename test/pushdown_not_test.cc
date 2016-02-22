// Copyright 2016 Chris Drake
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/optional.hpp>
#include <gtest/gtest.h>

#include "boolexpr/boolexpr.h"
#include "boolexprtest.h"


using namespace boolexpr;


class PushDownNotTest : public BoolExprTest {};


TEST_F(PushDownNotTest, Basic)
{
    auto y0 = ~or_({~xs[0], xs[1], ~xs[2], xs[3]});
    EXPECT_EQ(str(pushdown_not(y0)), "And(x_0, ~x_1, x_2, ~x_3)");

    auto y1 = ~and_({~xs[0], xs[1], ~xs[2], xs[3]});
    EXPECT_EQ(str(pushdown_not(y1)), "Or(x_0, ~x_1, x_2, ~x_3)");

    auto y2 = ~xor_({~xs[0], xs[1], ~xs[2], xs[3]});
    EXPECT_EQ(str(pushdown_not(y2)), "Xor(x_0, x_1, ~x_2, x_3)");

    auto y3 = ~eq({~xs[0], xs[1], ~xs[2], xs[3]});
    EXPECT_EQ(str(pushdown_not(y3)), "Equal(x_0, x_1, ~x_2, x_3)");

    auto y4 = ~impl(xs[0], xs[1]);
    EXPECT_EQ(str(pushdown_not(y4)), "And(x_0, ~x_1)");

    auto y5 = ~ite(xs[0], xs[1], xs[2]);
    EXPECT_EQ(str(pushdown_not(y5)), "IfThenElse(x_0, ~x_1, ~x_2)");
}


TEST_F(PushDownNotTest, TwoLevel)
{
    auto y0 = or_({~(xs[0] & xs[1]), ~(xs[2] & xs[3])});
    EXPECT_EQ(str(pushdown_not(y0)), "Or(Or(~x_0, ~x_1), Or(~x_2, ~x_3))");

    auto y1 = and_({~(xs[0] | xs[1]), ~(xs[2] | xs[3])});
    EXPECT_EQ(str(pushdown_not(y1)), "And(And(~x_0, ~x_1), And(~x_2, ~x_3))");

    auto y2 = xor_({~(xs[0] | xs[1]), ~(xs[2] | xs[3])});
    EXPECT_EQ(str(pushdown_not(y2)), "Xor(And(~x_0, ~x_1), And(~x_2, ~x_3))");

    auto y3 = eq({~(xs[0] & xs[1]), ~(xs[2] & xs[3])});
    EXPECT_EQ(str(pushdown_not(y3)), "Equal(Or(~x_0, ~x_1), Or(~x_2, ~x_3))");

    auto y4 = impl(~(xs[0] | xs[1]), ~(xs[2] | xs[3]));
    EXPECT_EQ(str(pushdown_not(y4)), "Or(Or(x_0, x_1), And(~x_2, ~x_3))");

    auto y5 = ite(~(xs[0] & xs[1]), ~(xs[2] & xs[3]), ~(xs[4] & xs[5]));
    EXPECT_EQ(str(pushdown_not(y5)), "IfThenElse(Or(~x_0, ~x_1), Or(~x_2, ~x_3), Or(~x_4, ~x_5))");
}
