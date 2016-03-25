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


#include <cstring>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/optional.hpp>

#include "boolexpr/boolexpr.h"


using namespace boolexpr;


struct BoolExprProxy
{
    bx_t const bx;

    BoolExprProxy(bx_t const & bx): bx {bx} {}
    ~BoolExprProxy() {}
};


void *
boolexpr_Context_new()
{
    return new Context();
}


void
boolexpr_Context_del(void * c_self)
{
    auto self = reinterpret_cast<Context *>(c_self);
    delete self;
}


void const *
boolexpr_Context_get_var(void * c_self, char const * c_name)
{
    auto self = reinterpret_cast<Context *>(c_self);
    std::string name { c_name };

    auto var = self->get_var(name);
    auto bx = std::static_pointer_cast<BoolExpr const>(var);
    return new BoolExprProxy(bx);
}


struct StringProxy
{
    char * str;

    StringProxy(string const & s)
    {
        str = new char [s.length() + 1];
        std::strcpy(str, s.c_str());
    }

    ~StringProxy()
    {
        delete [] str;
    }
};


void
boolexpr_String_del(void const *c_self)
{
    auto self = reinterpret_cast<StringProxy const *>(c_self);
    delete self;
}


char const *
boolexpr_String_str(void const *c_self)
{
    auto self = reinterpret_cast<StringProxy const *>(c_self);
    return self->str;
}


void const *
boolexpr_zero()
{ return new BoolExprProxy(zero()); }

void const *
boolexpr_one()
{ return new BoolExprProxy(one()); }

void const *
boolexpr_logical()
{ return new BoolExprProxy(logical()); }

void const *
boolexpr_illogical()
{ return new BoolExprProxy(illogical()); }


void const *
boolexpr_not(void const * c_arg)
{
    auto arg = reinterpret_cast<BoolExprProxy const *>(c_arg);
    return new BoolExprProxy(~arg->bx);
}


static vector<bx_t>
_convert_args(uint32_t n, void const * c_args[])
{
    vector<bx_t> _args(n);
    for (uint32_t i = 0u; i < n; ++i) {
        auto arg = reinterpret_cast<BoolExprProxy const *>(c_args[i]);
        _args[i] = arg->bx;
    }
    return std::move(_args);
}


void const *
boolexpr_nor(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(nor(_convert_args(n, c_args))); }

void const *
boolexpr_or(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(or_(_convert_args(n, c_args))); }

void const *
boolexpr_nand(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(nand(_convert_args(n, c_args))); }

void const *
boolexpr_and(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(and_(_convert_args(n, c_args))); }

void const *
boolexpr_xnor(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(xnor(_convert_args(n, c_args))); }

void const *
boolexpr_xor(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(xor_(_convert_args(n, c_args))); }

void const *
boolexpr_neq(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(neq(_convert_args(n, c_args))); }

void const *
boolexpr_eq(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(eq(_convert_args(n, c_args))); }

void const *
boolexpr_impl(void const * c_p, void const * c_q)
{
    auto p = reinterpret_cast<BoolExprProxy const *>(c_p);
    auto q = reinterpret_cast<BoolExprProxy const *>(c_q);
    return new BoolExprProxy(impl(p->bx, q->bx));
}

void const *
boolexpr_ite(void const * c_s, void const * c_d1, void const * c_d0)
{
    auto s = reinterpret_cast<BoolExprProxy const *>(c_s);
    auto d1 = reinterpret_cast<BoolExprProxy const *>(c_d1);
    auto d0 = reinterpret_cast<BoolExprProxy const *>(c_d0);
    return new BoolExprProxy(ite(s->bx, d1->bx, d0->bx));
}


void const *
boolexpr_nor_s(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(nor_s(_convert_args(n, c_args))); }

void const *
boolexpr_or_s(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(or_s(_convert_args(n, c_args))); }

void const *
boolexpr_nand_s(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(nand_s(_convert_args(n, c_args))); }

void const *
boolexpr_and_s(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(and_s(_convert_args(n, c_args))); }

void const *
boolexpr_xnor_s(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(xnor_s(_convert_args(n, c_args))); }

void const *
boolexpr_xor_s(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(xor_s(_convert_args(n, c_args))); }

void const *
boolexpr_neq_s(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(neq_s(_convert_args(n, c_args))); }

void const *
boolexpr_eq_s(uint32_t n, void const * c_args[])
{ return new BoolExprProxy(eq_s(_convert_args(n, c_args))); }

void const *
boolexpr_impl_s(void const * c_p, void const * c_q)
{
    auto p = reinterpret_cast<BoolExprProxy const *>(c_p);
    auto q = reinterpret_cast<BoolExprProxy const *>(c_q);
    return new BoolExprProxy(impl_s(p->bx, q->bx));
}

void const *
boolexpr_ite_s(void const * c_s, void const * c_d1, void const * c_d0)
{
    auto s = reinterpret_cast<BoolExprProxy const *>(c_s);
    auto d1 = reinterpret_cast<BoolExprProxy const *>(c_d1);
    auto d0 = reinterpret_cast<BoolExprProxy const *>(c_d0);
    return new BoolExprProxy(ite_s(s->bx, d1->bx, d0->bx));
}


void
boolexpr_BoolExpr_del(void const *c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    delete self;
}


uint32_t
boolexpr_BoolExpr_kind(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return static_cast<uint32_t>(self->bx->kind);
}


void const *
boolexpr_BoolExpr_to_string(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new StringProxy(self->bx->to_string());
}


uint32_t
boolexpr_BoolExpr_depth(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return self->bx->depth();
}


uint32_t
boolexpr_BoolExpr_size(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return self->bx->size();
}


uint32_t
boolexpr_BoolExpr_atom_count(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return self->bx->atom_count();
}


uint32_t
boolexpr_BoolExpr_op_count(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return self->bx->op_count();
}


bool
boolexpr_BoolExpr_is_cnf(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return self->bx->is_cnf();
}


bool
boolexpr_BoolExpr_is_dnf(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return self->bx->is_dnf();
}


void const *
boolexpr_BoolExpr_pushdown_not(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->pushdown_not());
}


void const *
boolexpr_BoolExpr_simplify(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->simplify());
}


void const *
boolexpr_BoolExpr_to_binop(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->to_binop());
}


void const *
boolexpr_BoolExpr_to_latop(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->to_latop());
}


void const *
boolexpr_BoolExpr_tseytin(void const * c_self, void * c_ctx, char const * c_auxvarname)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto ctx = reinterpret_cast<Context *>(c_ctx);
    std::string auxvarname { c_auxvarname };
    return new BoolExprProxy(self->bx->tseytin(*ctx, auxvarname));
}


void const *
boolexpr_BoolExpr_compose(void const * c_self, int n, void const ** c_varps, void const ** c_bxps)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto var2bx = var2bx_t();
    for (int i = 0; i < n; ++i) {
        auto varp = reinterpret_cast<BoolExprProxy const *>(c_varps[i]);
        auto bxp = reinterpret_cast<BoolExprProxy const *>(c_bxps[i]);
        auto var = std::static_pointer_cast<Variable const>(varp->bx);
        auto bx = bxp->bx;
        var2bx.insert({var, bx});
    }
    return new BoolExprProxy(self->bx->compose(var2bx));
}


void const *
boolexpr_BoolExpr_restrict(void const * c_self, int n, void const ** c_varps, void const ** c_constps)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto point = point_t();
    for (int i = 0; i < n; ++i) {
        auto varp = reinterpret_cast<BoolExprProxy const *>(c_varps[i]);
        auto constp = reinterpret_cast<BoolExprProxy const *>(c_constps[i]);
        auto var = std::static_pointer_cast<Variable const>(varp->bx);
        auto const_ = std::static_pointer_cast<Constant const>(constp->bx);
        point.insert({var, const_});
    }
    return new BoolExprProxy(self->bx->restrict_(point));
}


void const *
boolexpr_BoolExpr_to_cnf(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->to_cnf());
}


void const *
boolexpr_BoolExpr_to_dnf(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->to_dnf());
}


void const *
boolexpr_BoolExpr_to_nnf(void const * c_self)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    return new BoolExprProxy(self->bx->to_nnf());
}


bool
boolexpr_BoolExpr_equiv(void const * c_self, void const * c_other)
{
    auto self = reinterpret_cast<BoolExprProxy const *>(c_self);
    auto other = reinterpret_cast<BoolExprProxy const *>(c_other);
    return self->bx->equiv(other->bx);
}
