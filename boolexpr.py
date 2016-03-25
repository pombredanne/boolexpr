# Copyright 2016 Chris Drake
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


from _boolexpr import ffi, lib


def _expect_bx(obj):
    if obj == False:
        return lib.boolexpr_zero()
    elif obj == True:
        return lib.boolexpr_one()
    elif obj == "x" or obj == "X":
        return lib.boolexpr_logical()
    elif obj == "?":
        return lib.boolexpr_illogical()
    elif isinstance(obj, BoolExpr):
        return obj.cdata
    else:
        raise TypeError("Expected obj to be a BoolExpr")


def _expect_const(obj):
    if obj == False:
        return lib.boolexpr_zero()
    elif obj == True:
        return lib.boolexpr_one()
    elif obj == "x" or obj == "X":
        return lib.boolexpr_logical()
    elif obj == "?":
        return lib.boolexpr_illogical()
    elif isinstance(obj, Constant):
        return obj.cdata
    else:
        raise TypeError("Expected obj to be a Constant")


def _expect_var(obj):
    if isinstance(obj, Variable):
        return obj.cdata
    else:
        raise TypeError("Expected obj to be a Variable")


def _convert_args(args):
    n = len(args)
    _args = ffi.new("void const * []", n)
    for i, arg in enumerate(args):
        _args[i] = _expect_bx(arg)
    return n, _args


class Context:
    """
    A context for Boolean variables
    """
    def __init__(self):
        self._cdata = lib.boolexpr_Context_new()

    def __del__(self):
        lib.boolexpr_Context_del(self._cdata)

    @property
    def cdata(self):
        return self._cdata

    def get_var(self, name):
        cdata = lib.boolexpr_Context_get_var(self._cdata, name.encode("ascii"))
        return _bx(cdata)


class BoolExpr:
    """
    Base class for Boolean expressions
    """
    def __init__(self, cdata):
        self._cdata = cdata

    def __del__(self):
        lib.boolexpr_BoolExpr_del(self._cdata)

    @property
    def cdata(self):
        return self._cdata

    def __repr__(self):
        return self.__str__()

    def __bytes__(self):
        sp = lib.boolexpr_BoolExpr_to_string(self._cdata)
        try:
            b = ffi.string(lib.boolexpr_String_str(sp))
        finally:
            lib.boolexpr_String_del(sp)
        return b

    def __str__(self):
        return self.__bytes__().decode('utf-8')

    def __invert__(self):
        return not_(self)

    def __or__(self, other):
        return or_(self, other)

    def __ror__(self, other):
        return or_(other, self)

    def __and__(self, other):
        return and_(self, other)

    def __rand__(self, other):
        return and_(other, self)

    def __xor__(self, other):
        return xor(self, other)

    def __rxor__(self, other):
        return xor(other, self)

    @property
    def kind(self):
        return lib.boolexpr_BoolExpr_kind(self._cdata)

    def depth(self):
        return lib.boolexpr_BoolExpr_depth(self._cdata)

    def size(self):
        return lib.boolexpr_BoolExpr_size(self._cdata)

    def atom_count(self):
        return lib.boolexpr_BoolExpr_atom_count(self._cdata)

    def op_count(self):
        return lib.boolexpr_BoolExpr_op_count(self._cdata)

    def is_cnf(self):
        return bool(lib.boolexpr_BoolExpr_is_cnf(self._cdata))

    def is_dnf(self):
        return bool(lib.boolexpr_BoolExpr_is_dnf(self._cdata))

    def pushdown_not(self):
        return _bx(lib.boolexpr_BoolExpr_pushdown_not(self._cdata))

    def simplify(self):
        return _bx(lib.boolexpr_BoolExpr_simplify(self._cdata))

    def to_binop(self):
        return _bx(lib.boolexpr_BoolExpr_to_binop(self._cdata))

    def to_latop(self):
        return _bx(lib.boolexpr_BoolExpr_to_latop(self._cdata))

    def tseytin(self, ctx, auxvarname="a"):
        name = auxvarname.encode("ascii")
        return _bx(lib.boolexpr_BoolExpr_tseytin(self._cdata, ctx.cdata, name))

    def compose(self, var2bx):
        n = len(var2bx)
        vars_ = ffi.new("void * []", n)
        bxs = ffi.new("void * []", n)
        for i, (var, bx) in enumerate(var2bx.items()):
            vars_[i] = _expect_var(var)
            bxs[i] = _expect_bx(bx)
        return _bx(lib.boolexpr_BoolExpr_compose(self._cdata, n, vars_, bxs))

    def restrict(self, point):
        n = len(point)
        vars_ = ffi.new("void * []", n)
        consts = ffi.new("void * []", n)
        for i, (var, const) in enumerate(point.items()):
            vars_[i] = _expect_var(var)
            consts = _expect_const(const)
        return _bx(lib.boolexpr_BoolExpr_restrict(self._cdata, n, vars_, consts))

    def to_cnf(self):
        return _bx(lib.boolexpr_BoolExpr_to_cnf(self._cdata))

    def to_dnf(self):
        return _bx(lib.boolexpr_BoolExpr_to_dnf(self._cdata))

    def to_nnf(self):
        return _bx(lib.boolexpr_BoolExpr_to_nnf(self._cdata))

    def equiv(self, other):
        return bool(lib.boolexpr_BoolExpr_equiv(self._cdata, other.cdata))

    def support(self):
        ret = set()
        s = lib.boolexpr_BoolExpr_support(self._cdata)
        try:
            lib.boolexpr_VarSet_iter(s)
            while True:
                val = lib.boolexpr_VarSet_val(s)
                if val == ffi.NULL:
                    break
                ret.add(_bx(val))
                lib.boolexpr_VarSet_next(s)
        finally:
            lib.boolexpr_VarSet_del(s)
        return ret

    def dfs_iter(self):
        it = lib.boolexpr_DfsIter_new(self._cdata)
        try:
            lib.boolexpr_DfsIter_iter(it)
            while True:
                val = lib.boolexpr_DfsIter_val(it)
                if val == ffi.NULL:
                    break
                yield _bx(val)
                lib.boolexpr_DfsIter_next(it)
        finally:
            lib.boolexpr_DfsIter_del(it)


class Atom(BoolExpr): pass
class Constant(Atom): pass
class Known(Constant): pass
class Zero(Known): pass
class One(Known): pass
class Unknown(Constant): pass
class Logical(Unknown): pass
class Illogical(Unknown): pass
class Literal(Atom): pass
class Complement(Literal): pass
class Variable(Literal): pass
class Operator(BoolExpr): pass
class LatticeOperator(Operator): pass
class Nor(Operator): pass
class Or(LatticeOperator): pass
class Nand(Operator): pass
class And(LatticeOperator): pass
class Xnor(Operator): pass
class Xor(Operator): pass
class Unequal(Operator): pass
class Equal(Operator): pass
class NotImplies(Operator): pass
class Implies(Operator): pass
class NotIfThenElse(Operator): pass
class IfThenElse(Operator): pass


_KIND2CLS = {
    0x00: Zero,
    0x01: One,
    0x04: Logical,
    0x06: Illogical,
    0x08: Complement,
    0x09: Variable,
    0x10: Nor,
    0x11: Or,
    0x12: Nand,
    0x13: And,
    0x14: Xnor,
    0x15: Xor,
    0x16: Unequal,
    0x17: Equal,
    0x18: NotImplies,
    0x19: Implies,
    0x1A: NotIfThenElse,
    0x1B: IfThenElse,
}

def _bx(cbx):
    kind = lib.boolexpr_BoolExpr_kind(cbx)
    return _KIND2CLS[kind](cbx)


def not_(arg):
    n, args = _convert_args((arg, ))
    return _bx(lib.boolexpr_not(arg.cdata))

def nor(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_nor(n, args))

def or_(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_or(n, args))

def nand(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_nand(n, args))

def and_(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_and(n, args))

def xnor(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_xnor(n, args))

def xor(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_xor(n, args))

def neq(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_neq(n, args))

def eq(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_eq(n, args))

def impl(p, q):
    _, args = _convert_args((p, q))
    return _bx(lib.boolexpr_impl(args[0], args[1]))

def ite(s, d1, d0):
    _, args = _convert_args((s, d1, d0))
    return _bx(lib.boolexpr_ite(args[0], args[1], args[2]))

def nor_s(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_nor_s(n, args))

def or_s(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_or_s(n, args))

def nand_s(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_nand_s(n, args))

def and_s(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_and_s(n, args))

def xnor_s(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_xnor_s(n, args))

def xor_s(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_xor_s(n, args))

def neq_s(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_neq_s(n, args))

def eq_s(*args):
    n, args = _convert_args(args)
    return _bx(lib.boolexpr_eq_s(n, args))

def impl_s(p, q):
    _, args = _convert_args((p, q))
    return _bx(lib.boolexpr_impl_s(args[0], args[1]))

def ite_s(s, d1, d0):
    _, args = _convert_args((s, d1, d0))
    return _bx(lib.boolexpr_ite_s(args[0], args[1], args[2]))
