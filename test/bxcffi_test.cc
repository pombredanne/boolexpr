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


struct BoolExprProxy {
    bx_t const bx;

    BoolExprProxy(bx_t const & bx): bx {bx} {}
    ~BoolExprProxy() {}
};


TEST(CFFI, Basic)
{
    auto zero = boolexpr_zero();
    auto one = boolexpr_one();

    EXPECT_TRUE(strcmp(boolexpr_BoolExpr_to_string(zero), "0") == 0);
    EXPECT_TRUE(strcmp(boolexpr_BoolExpr_to_string(one), "1") == 0);

    auto ctx = boolexpr_Context_new();

    auto a = boolexpr_Context_get_var(ctx, "a");
    auto b = boolexpr_Context_get_var(ctx, "b");
    auto c = boolexpr_Context_get_var(ctx, "c");
    auto d = boolexpr_Context_get_var(ctx, "d");

    auto an = boolexpr_not(a);
    auto bn = boolexpr_not(b);
    auto cn = boolexpr_not(c);
    auto dn = boolexpr_not(d);

    EXPECT_TRUE(strcmp(boolexpr_BoolExpr_to_string(a), "a") == 0);
    EXPECT_TRUE(strcmp(boolexpr_BoolExpr_to_string(b), "b") == 0);
    EXPECT_TRUE(strcmp(boolexpr_BoolExpr_to_string(c), "c") == 0);
    EXPECT_TRUE(strcmp(boolexpr_BoolExpr_to_string(d), "d") == 0);

    EXPECT_TRUE(strcmp(boolexpr_BoolExpr_to_string(an), "~a") == 0);
    EXPECT_TRUE(strcmp(boolexpr_BoolExpr_to_string(bn), "~b") == 0);
    EXPECT_TRUE(strcmp(boolexpr_BoolExpr_to_string(cn), "~c") == 0);
    EXPECT_TRUE(strcmp(boolexpr_BoolExpr_to_string(dn), "~d") == 0);

    void const * args[] = {an, b, cn, d};

    auto y0 = boolexpr_or(4, args);
    auto y1 = boolexpr_and(4, args);
    auto y2 = boolexpr_xor(4, args);
    auto y3 = boolexpr_eq(4, args);
    auto y4 = boolexpr_impl(an, b);
    auto y5 = boolexpr_ite(an, b, cn);

    EXPECT_EQ(boolexpr_BoolExpr_kind(y0), BoolExpr::OR);
    EXPECT_EQ(boolexpr_BoolExpr_kind(y1), BoolExpr::AND);
    EXPECT_EQ(boolexpr_BoolExpr_kind(y2), BoolExpr::XOR);
    EXPECT_EQ(boolexpr_BoolExpr_kind(y3), BoolExpr::EQ);
    EXPECT_EQ(boolexpr_BoolExpr_kind(y4), BoolExpr::IMPL);
    EXPECT_EQ(boolexpr_BoolExpr_kind(y5), BoolExpr::ITE);

    EXPECT_EQ(boolexpr_BoolExpr_depth(y0), 1);
    EXPECT_EQ(boolexpr_BoolExpr_depth(y1), 1);
    EXPECT_EQ(boolexpr_BoolExpr_depth(y2), 1);
    EXPECT_EQ(boolexpr_BoolExpr_depth(y3), 1);
    EXPECT_EQ(boolexpr_BoolExpr_depth(y4), 1);
    EXPECT_EQ(boolexpr_BoolExpr_depth(y5), 1);

    char const *y0_s = boolexpr_BoolExpr_to_string(y0);
    char const *y1_s = boolexpr_BoolExpr_to_string(y1);
    char const *y2_s = boolexpr_BoolExpr_to_string(y2);
    char const *y3_s = boolexpr_BoolExpr_to_string(y3);
    char const *y4_s = boolexpr_BoolExpr_to_string(y4);
    char const *y5_s = boolexpr_BoolExpr_to_string(y5);

    EXPECT_TRUE(strcmp(y0_s, "Or(~a, b, ~c, d)") == 0);
    EXPECT_TRUE(strcmp(y1_s, "And(~a, b, ~c, d)") == 0);
    EXPECT_TRUE(strcmp(y2_s, "Xor(~a, b, ~c, d)") == 0);
    EXPECT_TRUE(strcmp(y3_s, "Equal(~a, b, ~c, d)") == 0);
    EXPECT_TRUE(strcmp(y4_s, "Implies(~a, b)") == 0);
    EXPECT_TRUE(strcmp(y5_s, "IfThenElse(~a, b, ~c)") == 0);
}