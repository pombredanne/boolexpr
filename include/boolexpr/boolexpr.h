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


#ifndef BOOLEXPR_H
#define BOOLEXPR_H


// Kind checks
#define IS_ZERO(expr)  ((expr)->kind == boolexpr::ZERO)
#define IS_ONE(expr)   ((expr)->kind == boolexpr::ONE)
#define IS_COMP(expr)  ((expr)->kind == boolexpr::COMP)
#define IS_VAR(expr)   ((expr)->kind == boolexpr::VAR)
#define IS_NOR(expr)   ((expr)->kind == boolexpr::NOR)
#define IS_OR(expr)    ((expr)->kind == boolexpr::OR)
#define IS_NAND(expr)  ((expr)->kind == boolexpr::NAND)
#define IS_AND(expr)   ((expr)->kind == boolexpr::AND)
#define IS_XNOR(expr)  ((expr)->kind == boolexpr::XNOR)
#define IS_XOR(expr)   ((expr)->kind == boolexpr::XOR)
#define IS_NEQ(expr)   ((expr)->kind == boolexpr::NEQ)
#define IS_EQ(expr)    ((expr)->kind == boolexpr::EQ)
#define IS_NIMPL(expr) ((expr)->kind == boolexpr::NIMPL)
#define IS_IMPL(expr)  ((expr)->kind == boolexpr::IMPL)
#define IS_NITE(expr)  ((expr)->kind == boolexpr::NITE)
#define IS_ITE(expr)   ((expr)->kind == boolexpr::ITE)

#define ARE_SAME(ex1, ex2) ((ex1)->kind == (ex2)->kind)

// Category checks
#define IS_ATOM(expr)    ((expr)->kind >> 4 == 0) // 0 ****
#define IS_OP(expr)      ((expr)->kind >> 4 == 1) // 1 ****
#define IS_CONST(expr)   ((expr)->kind >> 3 == 0) // 0 0***
#define IS_KNOWN(expr)   ((expr)->kind >> 2 == 0) // 0 00**
#define IS_UNKNOWN(expr) ((expr)->kind >> 2 == 1) // 0 01**
#define IS_LIT(expr)     ((expr)->kind >> 3 == 1) // 0 1***
#define IS_NARY(expr)    ((expr)->kind >> 3 == 2) // 1 0***
#define IS_NEG(expr)     (!((expr)->kind & 1))    // * ***0
#define IS_POS(expr)     ((expr)->kind & 1)       // * ***1


namespace boolexpr {


using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;


// Forward declarations
class BoolExpr;
class Constant;
class Zero;
class One;
class Logical;
class Illogical;
class Literal;
class Variable;
class Operator;
class LatticeOperator;
class Context;


// Expression kinds
typedef enum {
    ZERO = 0x00,        // 0 0000
    ONE  = 0x01,        // 0 0001

    LOGICAL   = 0x04,   // 0 0100
    ILLOGICAL = 0x06,   // 0 0110

    COMP = 0x08,        // 0 1000
    VAR  = 0x09,        // 0 1001

    NOR  = 0x10,        // 1 0000
    OR   = 0x11,        // 1 0001
    NAND = 0x12,        // 1 0010
    AND  = 0x13,        // 1 0011

    XNOR = 0x14,        // 1 0100
    XOR  = 0x15,        // 1 0101
    NEQ  = 0x16,        // 1 0110
    EQ   = 0x17,        // 1 0111

    NIMPL = 0x18,       // 1 1000
    IMPL  = 0x19,       // 1 1001

    NITE = 0x1A,        // 1 1010
    ITE  = 0x1B,        // 1 1011
} Kind;


using id_t = uint32_t;

using bx_t = std::shared_ptr<BoolExpr>;

using const_t = std::shared_ptr<Constant>;
using zero_t = std::shared_ptr<Zero>;
using one_t = std::shared_ptr<One>;
using log_t = std::shared_ptr<Logical>;
using ill_t = std::shared_ptr<Illogical>;

using lit_t = std::shared_ptr<Literal>;
using var_t = std::shared_ptr<Variable>;
using op_t = std::shared_ptr<Operator>;
using lop_t = std::shared_ptr<LatticeOperator>;

using var2bx_t = unordered_map<var_t, bx_t>;
using var2op_t = unordered_map<var_t, op_t>;
using point_t = unordered_map<var_t, const_t>;

using soln_t = std::pair<bool, boost::optional<point_t>>;


class BoolExpr {
protected:
    virtual string _str(const bx_t&) const = 0;
    virtual bx_t _invert(const bx_t&) const = 0;
    virtual bx_t _pushdown_not(const bx_t&) const = 0;
    virtual bx_t _simplify(const bx_t&) const = 0;
    virtual bx_t _to_latop(const bx_t&) const = 0;
    virtual bx_t _to_binop(const bx_t&) const = 0;
    virtual bx_t _compose(const bx_t&, const var2bx_t&) const = 0;
    virtual bx_t _restrict(const bx_t&, const point_t&) const = 0;
    virtual bx_t _tseytin(const bx_t&, Context&, const string&, uint32_t&, var2op_t&) const = 0;
    virtual soln_t _sat(const bx_t&) const = 0;

public:
    const Kind kind;

    BoolExpr(Kind kind);

    friend bool operator<(const bx_t&, const bx_t&);
    friend string str(const bx_t&);
    friend bx_t invert(const bx_t&);
    friend bx_t pushdown_not(const bx_t&);
    friend bx_t simplify(const bx_t&);
    friend bx_t to_latop(const bx_t&);
    friend bx_t to_binop(const bx_t&);
    friend bx_t to_nnf(const bx_t&);
    friend bx_t to_cnf(const bx_t&);
    friend bx_t to_dnf(const bx_t&);
    friend bx_t compose(const bx_t&, const var2bx_t&);
    friend bx_t restrict_(const bx_t&, const point_t&);
    friend bx_t tseytin(const bx_t&, Context&, const string&);
    friend soln_t sat(const bx_t&);

    virtual uint32_t depth() const = 0;
    virtual uint32_t size() const = 0;
    virtual uint32_t atom_count() const = 0;
    virtual uint32_t op_count() const = 0;

    virtual bool is_dnf() const = 0;
    virtual bool is_cnf() const = 0;
};


class Atom : public BoolExpr {
protected:
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_latop(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;
    bx_t _tseytin(const bx_t&, Context&, const string&, uint32_t&, var2op_t&) const;

public:
    Atom(Kind kind);

    uint32_t depth() const;
    uint32_t size() const;
    uint32_t atom_count() const;
    uint32_t op_count() const;

    bool is_dnf() const;
    bool is_cnf() const;
};


class Constant : public Atom {
protected:
    bx_t _compose(const bx_t&, const var2bx_t&) const;
    bx_t _restrict(const bx_t&, const point_t&) const;

public:
    Constant(Kind kind);
};


class Known : public Constant {
public:
    const bool val;

    Known(Kind kind, bool val);
};


class Zero : public Known {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    soln_t _sat(const bx_t&) const;

public:
    Zero();

    bool is_dnf() const;
};


class One : public Known {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    soln_t _sat(const bx_t&) const;

public:
    One();

    bool is_cnf() const;
};


class Unknown : public Constant {
public:
    Unknown(Kind kind);
};


class Logical : public Unknown {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    soln_t _sat(const bx_t&) const;

public:
    Logical();
};


class Illogical : public Unknown {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    soln_t _sat(const bx_t&) const;

public:
    Illogical();
};


class Literal : public Atom {
public:
    Context *ctx;
    const id_t id;

    Literal(Kind kind, Context *ctx, id_t id);

    bool is_dnf() const;
    bool is_cnf() const;
};


class Complement : public Literal {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _compose(const bx_t&, const var2bx_t&) const;
    bx_t _restrict(const bx_t&, const point_t&) const;
    soln_t _sat(const bx_t&) const;

public:
    Complement(Context *ctx, id_t id);
};


class Variable : public Literal {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _compose(const bx_t&, const var2bx_t&) const;
    bx_t _restrict(const bx_t&, const point_t&) const;
    soln_t _sat(const bx_t&) const;

public:
    Variable(Context *ctx, id_t id);
};


class Operator : public BoolExpr {
protected:
    string _tostr(const string) const;
    bx_t _compose(const bx_t&, const var2bx_t&) const;
    bx_t _restrict(const bx_t&, const point_t&) const;
    bx_t _tseytin(const bx_t&, Context&, const string&, uint32_t&, var2op_t&) const;
    soln_t _sat(const bx_t&) const;

public:
    const bool simple;
    const vector<bx_t> args;

    Operator(Kind kind, bool simple, const vector<bx_t>& args);
    Operator(Kind kind, bool simple, const vector<bx_t>&& args);

    uint32_t depth() const;
    uint32_t size() const;
    uint32_t atom_count() const;
    uint32_t op_count() const;

    bool is_d1() const;
    bool is_clause() const;
    bool is_dnf() const;
    bool is_cnf() const;

    virtual op_t from_args(const vector<bx_t>&) const = 0;
    virtual op_t from_args(const vector<bx_t>&&) const = 0;
    virtual bx_t eqvar(const var_t&) const = 0;
};


class LatticeOperator : public Operator {
protected:
    bx_t _to_latop(const bx_t&) const;

public:
    LatticeOperator(Kind kind, bool simple, const vector<bx_t>& args);
    LatticeOperator(Kind kind, bool simple, const vector<bx_t>&& args);
};


class Nor : public Operator {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_latop(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;

public:
    Nor(bool simple, const vector<bx_t>& args);
    Nor(bool simple, const vector<bx_t>&& args);

    op_t from_args(const vector<bx_t>&) const;
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(const var_t&) const;
};


class Or : public LatticeOperator {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;

public:
    Or(bool simple, const vector<bx_t>& args);
    Or(bool simple, const vector<bx_t>&& args);

    static bx_t identity();
    static bx_t dominator();

    bool is_dnf() const;
    bool is_cnf() const;

    op_t from_args(const vector<bx_t>&) const;
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(const var_t&) const;
};


class Nand : public Operator {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_latop(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;

public:
    Nand(bool simple, const vector<bx_t>& args);
    Nand(bool simple, const vector<bx_t>&& args);

    op_t from_args(const vector<bx_t>&) const;
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(const var_t&) const;
};


class And : public LatticeOperator {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;

public:
    And(bool simple, const vector<bx_t>& args);
    And(bool simple, const vector<bx_t>&& args);

    static bx_t identity();
    static bx_t dominator();

    bool is_dnf() const;
    bool is_cnf() const;

    op_t from_args(const vector<bx_t>&) const;
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(const var_t&) const;
};


class Xnor : public Operator {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_latop(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;

public:
    Xnor(bool simple, const vector<bx_t>& args);
    Xnor(bool simple, const vector<bx_t>&& args);

    op_t from_args(const vector<bx_t>&) const;
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(const var_t&) const;
};


class Xor : public Operator {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_latop(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;

public:
    Xor(bool simple, const vector<bx_t>& args);
    Xor(bool simple, const vector<bx_t>&& args);

    static bx_t identity();

    op_t from_args(const vector<bx_t>&) const;
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(const var_t&) const;
};


class Unequal : public Operator {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_latop(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;

public:
    Unequal(bool simple, const vector<bx_t>& args);
    Unequal(bool simple, const vector<bx_t>&& args);

    op_t from_args(const vector<bx_t>&) const;
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(const var_t&) const;
};


class Equal : public Operator {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_latop(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;

public:
    Equal(bool simple, const vector<bx_t>& args) : Operator(EQ, simple, args) {}
    Equal(bool simple, const vector<bx_t>&& args) : Operator(EQ, simple, args) {}

    op_t from_args(const vector<bx_t>&) const;
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(const var_t&) const;
};


class NotImplies : public Operator {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_latop(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;

public:
    NotImplies(bool simple, bx_t p, bx_t q);

    op_t from_args(const vector<bx_t>&) const;
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(const var_t&) const;
};


class Implies : public Operator {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_latop(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;

public:
    Implies(bool simple, bx_t p, bx_t q);

    op_t from_args(const vector<bx_t>&) const;
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(const var_t&) const;
};


class NotIfThenElse : public Operator {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_latop(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;

public:
    NotIfThenElse(bool simple, bx_t s, bx_t d1, bx_t d0);

    op_t from_args(const vector<bx_t>&) const;
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(const var_t&) const;
};


class IfThenElse : public Operator {
protected:
    string _str(const bx_t&) const;
    bx_t _invert(const bx_t&) const;
    bx_t _pushdown_not(const bx_t&) const;
    bx_t _simplify(const bx_t&) const;
    bx_t _to_latop(const bx_t&) const;
    bx_t _to_binop(const bx_t&) const;

public:
    IfThenElse(bool simple, bx_t s, bx_t d1, bx_t d0);

    op_t from_args(const vector<bx_t>&) const;
    op_t from_args(const vector<bx_t>&&) const;
    bx_t eqvar(const var_t&) const;
};


class dfs_iter : public std::iterator<std::input_iterator_tag, bx_t>
{
private:
    vector<bx_t> stack;

protected:
    enum class Color { WHITE, GRAY, BLACK };
    unordered_map<bx_t, Color> colors;
    const bx_t* p;
    void _advance_one();

public:
    dfs_iter();
    dfs_iter(const bx_t&);

    bool operator==(const dfs_iter&) const;
    bool operator!=(const dfs_iter&) const;
    const bx_t& operator*() const;
    const dfs_iter& operator++();
};


class Context {
private:
    id_t id;

    unordered_map<string, var_t> vars;
    unordered_map<id_t, string> id2name;
    unordered_map<id_t, lit_t> id2lit;

public:
    Context();

    var_t get_var(string name);
    string get_name(id_t id) const;
    lit_t get_lit(id_t id) const;
};


class ArgSet {
protected:
    unordered_set<bx_t> args;
    virtual void insert(const bx_t&) = 0;
    virtual bx_t to_op() const = 0;

public:
    virtual bx_t reduce() const = 0;
};


class LatticeArgSet : public ArgSet {
protected:
    bool infimum;
    bool supremum;
    Kind kind;
    bx_t identity;
    bx_t dominator;

    void insert(const bx_t&);

public:
    LatticeArgSet(const vector<bx_t>& args, const Kind& kind,
                  const bx_t& identity, const bx_t& dominator);
    bx_t reduce() const;
};


class OrArgSet : public LatticeArgSet {
protected:
    bx_t to_op() const;

public:
    OrArgSet(const vector<bx_t>& args);
};


class AndArgSet : public LatticeArgSet {
protected:
    bx_t to_op() const;

public:
    AndArgSet(const vector<bx_t>& args);
};


class XorArgSet : public ArgSet {
private:
    bool parity;

protected:
    void insert(const bx_t&);
    bx_t to_op() const;

public:
    XorArgSet(const vector<bx_t>& args);
    bx_t reduce() const;
};


class EqArgSet : public ArgSet {
private:
    bool has_zero;
    bool has_one;

protected:
    void insert(const bx_t&);
    bx_t to_op() const;

public:
    EqArgSet(const vector<bx_t>& args);
    bx_t reduce() const;
};


dfs_iter begin(const bx_t&);
const dfs_iter end(const bx_t&);

zero_t zero();
one_t one();
log_t logical();
ill_t illogical();

bx_t or_(const vector<bx_t>&);
bx_t or_(const vector<bx_t>&&);
bx_t or_(std::initializer_list<bx_t>);
bx_t and_(const vector<bx_t>&);
bx_t and_(const vector<bx_t>&&);
bx_t and_(std::initializer_list<bx_t>);
bx_t xor_(const vector<bx_t>&);
bx_t xor_(const vector<bx_t>&&);
bx_t xor_(std::initializer_list<bx_t>);
bx_t eq(const vector<bx_t>&);
bx_t eq(const vector<bx_t>&&);
bx_t eq(std::initializer_list<bx_t>);
bx_t impl(const bx_t&, const bx_t&);
bx_t ite(const bx_t&, const bx_t&, const bx_t&);

bx_t operator~(const bx_t&);
bx_t operator|(const bx_t&, const bx_t&);
bx_t operator&(const bx_t&, const bx_t&);
bx_t operator^(const bx_t&, const bx_t&);
bool operator<(const bx_t&, const bx_t&);
bool operator<(const lit_t&, const lit_t&);

string str(const bx_t&);
bx_t invert(const bx_t&);
bx_t pushdown_not(const bx_t&);
bx_t simplify(const bx_t&);
bx_t to_latop(const bx_t&);
bx_t to_binop(const bx_t&);
bx_t to_nnf(const bx_t&);
bx_t to_cnf(const bx_t&);
bx_t to_dnf(const bx_t&);
unordered_set<var_t> support(const bx_t&);
op_t transform(const op_t&, std::function<bx_t(const bx_t&)>);
vector<bx_t> cofactors(const bx_t&, vector<var_t>&);
bx_t smoothing(const bx_t&, vector<var_t>&);
bx_t consensus(const bx_t&, vector<var_t>&);
bx_t derivative(const bx_t&, vector<var_t>&);
bx_t compose(const bx_t&, const var2bx_t&);
bx_t restrict_(const bx_t&, const point_t&);
bx_t tseytin(const bx_t&, Context&, const string& = "a");
soln_t sat(const bx_t&);

bool equivalent(const bx_t&, const bx_t&);


} // namespace boolexpr


#endif /* BOOLEXPR_H */