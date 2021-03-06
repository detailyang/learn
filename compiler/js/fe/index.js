/*
* @Author: detailyang
* @Date:   2016-04-25 02:40:20
* @Last Modified by:   detailyang
* @Last Modified time: 2016-05-09 23:57:42
*/

'use strict';


function Input(input) {
    let pos = 0;
    let line = 1;
    let col = 0;

    return {
        next,
        peek,
        eof,
        croak,
    };

    function peek() {
        return input.charAt(pos);
    }

    function next() {
        const ch = input.charAt(pos++);
        if (ch === '\n') {
            line++;
            col = 0;
        } else {
            col += 1;
        }
        return ch;
    }

    function eof() {
        return peek() === "";
    }

    function croak(msg) {
        throw new Error(`${msg} (${line}:${col})`);
    }
}

function InputStream(input) {
    let pos = 0, line = 1, col = 0;
    let current = null;
    return {
        next,
        peek,
        eof,
        croak: input.croak,
    };

    function next() {
        const token = current;
        current = null;
        return token || read_next();
    }

    function is_whitespace(ch) {
        return " \t\n".indexOf(ch) !== -1;
    }

    function skip_comment() {
        read_while((ch) => {
            return ch != '\n';
        });
    }

    function is_keyword(x) {
        const keywords = " if then else lambda λ true false let ";
        return keywords.indexOf(" " + x + " ") >= 0;
    }

    function read_escaped(end) {
        let escaped = false;
        let str = "";
        input.next();
        while (!input.eof()) {
            const ch = input.next();
            if (escaped) {
                str += ch;
                escaped = false;
            } else if (ch == "\\") {
                escaped = true;
            } else if (ch == end) {
                break;
            } else {
                str += ch;
            }
        }

        return str;
    }

    function is_digit(ch) {
        return /[0-9]/i.test(ch);
    }

    function read_string() {
        return {
            type: "str",
            value: read_escaped('"')
        };
    }

    function is_op_char(ch) {
        return "+-*/%=&|<>!".indexOf(ch) !== -1;
    }

    function is_punc(ch) {
        return ",;(){}[]".indexOf(ch) !== -1;
    }

    function is_id_start(ch) {
        return /[a-zλ_]/i.test(ch);
    }

    function is_id(ch) {
        return is_id_start(ch) || "?!-<>=0123456789".indexOf(ch) !== -1;
    }

    function read_ident() {
        const id = read_while(is_id);
        return {
            type: is_keyword(id) ? "kw" : "var",
            value: id,
        };
    }

    function read_next() {
        read_while(is_whitespace);
        if (input.eof())  {
            return null;
        }
        const ch = input.peek();
        if (ch === '#') {
            skip_comment();
            return read_next();
        }
        if (ch === '"') {
            return read_string();
        }
        if (is_digit(ch)) {
            return read_number();
        }
        if (is_id_start(ch)) {
            return read_ident();
        }
        if (is_punc(ch)) {
            return {
                type: "punc",
                value: input.next(),
            };
        }
        if (is_op_char(ch)) {
            return {
                type: "op",
                value: read_while(is_op_char),
            };
        }
        input.croak(`Can't handle character: ${ch}`);
    }

    function read_number() {
        let has_dot = false;
        const number = read_while((ch) => {
            if (ch === ".") {
                if (has_dot) return false;
                has_dot = true;
                return true;
            }
            return is_digit(ch);
        });

        return {
            type: 'num',
            value: parseFloat((number))
        };
    }

    function read_while(predicate) {
        let str = "";
        while (!input.eof() && predicate(input.peek())) {
            str += input.next();
        }
        return str;
    }

    function peek() {
        return current || (current = read_next());
    }

    function eof() {
        return peek() == null;
    }
}
var FALSE = { type: "bool", value: false };
function parse(input) {
    var PRECEDENCE = {
        "=": 1,
        "||": 2,
        "&&": 3,
        "<": 7, ">": 7, "<=": 7, ">=": 7, "==": 7, "!=": 7,
        "+": 10, "-": 10,
        "*": 20, "/": 20, "%": 20,
    };
    return parse_toplevel();
    function is_punc(ch) {
        var tok = input.peek();
        return tok && tok.type == "punc" && (!ch || tok.value == ch) && tok;
    }
    function is_kw(kw) {
        var tok = input.peek();
        return tok && tok.type == "kw" && (!kw || tok.value == kw) && tok;
    }
    function is_op(op) {
        var tok = input.peek();
        return tok && tok.type == "op" && (!op || tok.value == op) && tok;
    }
    function skip_punc(ch) {
        if (is_punc(ch)) input.next();
        else input.croak("Expecting punctuation: \"" + ch + "\"");
    }
    function skip_kw(kw) {
        if (is_kw(kw)) input.next();
        else input.croak("Expecting keyword: \"" + kw + "\"");
    }
    function skip_op(op) {
        if (is_op(op)) input.next();
        else input.croak("Expecting operator: \"" + op + "\"");
    }
    function unexpected() {
        input.croak("Unexpected token: " + JSON.stringify(input.peek()));
    }
    function maybe_binary(left, my_prec) {
        var tok = is_op();
        if (tok) {
            var his_prec = PRECEDENCE[tok.value];
            if (his_prec > my_prec) {
                input.next();
                return maybe_binary({
                    type     : tok.value == "=" ? "assign" : "binary",
                    operator : tok.value,
                    left     : left,
                    right    : maybe_binary(parse_atom(), his_prec)
                }, my_prec);
            }
        }
        return left;
    }
    function delimited(start, stop, separator, parser) {
        var a = [], first = true;
        skip_punc(start);
        while (!input.eof()) {
            if (is_punc(stop)) break;
            if (first) first = false; else skip_punc(separator);
            if (is_punc(stop)) break;
            a.push(parser());
        }
        skip_punc(stop);
        return a;
    }
    function parse_call(func) {
        return {
            type: "call",
            func: func,
            args: delimited("(", ")", ",", parse_expression),
        };
    }
    function parse_varname() {
        var name = input.next();
        if (name.type != "var") input.croak("Expecting variable name");
        return name.value;
    }
    function parse_if() {
        skip_kw("if");
        var cond = parse_expression();
        if (!is_punc("{")) skip_kw("then");
        var then = parse_expression();
        var ret = {
            type: "if",
            cond: cond,
            then: then,
        };
        if (is_kw("else")) {
            input.next();
            ret.else = parse_expression();
        }
        return ret;
    }
    function parse_lambda() {
        return {
            type: "lambda",
            name: input.peek().type == "var" ? input.next().value : null, // this line
            vars: delimited("(", ")", ",", parse_varname),
            body: parse_expression()
        };
    }
    function parse_let() {
        skip_kw("let");
        if (input.peek().type == "var") {
            var name = input.next().value;
            var defs = delimited("(", ")", ",", parse_vardef);
            return {
                type: "call",
                func: {
                    type: "lambda",
                    name: name,
                    vars: defs.map(function(def){ return def.name }),
                    body: parse_expression(),
                },
                args: defs.map(function(def){ return def.def || FALSE })
            };
        }
        return {
            type: "let",
            vars: delimited("(", ")", ",", parse_vardef),
            body: parse_expression(),
        };
    }
    function parse_vardef() {
        var name = parse_varname(), def;
        if (is_op("=")) {
            input.next();
            def = parse_expression();
        }
        return { name: name, def: def };
    }
    function parse_bool() {
        return {
            type  : "bool",
            value : input.next().value == "true"
        };
    }
    function maybe_call(expr) {
        expr = expr();
        return is_punc("(") ? parse_call(expr) : expr;
    }
    function parse_atom() {
        return maybe_call(function(){
            if (is_punc("(")) {
                input.next();
                var exp = parse_expression();
                skip_punc(")");
                return exp;
            }
            if (is_punc("{")) return parse_prog();
            if (is_kw("let")) return parse_let();
            if (is_kw("if")) return parse_if();
            if (is_kw("true") || is_kw("false")) return parse_bool();
            if (is_kw("lambda") || is_kw("λ")) {
                input.next();
                return parse_lambda();
            }
            var tok = input.next();
            if (tok.type == "var" || tok.type == "num" || tok.type == "str")
                return tok;
            unexpected();
        });
    }
    function parse_toplevel() {
        var prog = [];
        while (!input.eof()) {
            prog.push(parse_expression());
            if (!input.eof()) skip_punc(";");
        }
        return { type: "prog", prog: prog };
    }
    function parse_prog() {
        var prog = delimited("{", "}", ";", parse_expression);
        if (prog.length == 0) return FALSE;
        if (prog.length == 1) return prog[0];
        return { type: "prog", prog: prog };
    }
    function parse_expression() {
        return maybe_call(function(){
            return maybe_binary(parse_atom(), 0);
        });
    }
}


function myparse(input) {
    var PRECEDENCE = {
        "=": 1,
        "||": 2,
        "&&": 3,
        "<": 7, ">": 7, "<=": 7, ">=": 7, "==": 7, "!=": 7,
        "+": 10, "-": 10,
        "*": 20, "/": 20, "%": 20,
    };
    return parse_toplevel();
    function is_punc(ch) {
        const tok = input.peek();
        return tok && tok.type == "punc" && (!ch || tok.value == ch) && tok;
    }
    function is_op(op) {
        var tok = input.peek();
        return tok && tok.type == "op" && (!op || tok.value == op) && tok;
    }
    function skip_punc(ch) {
        if (is_punc(ch)) {
            input.next();
        } else {
            input.croak("Expecting punctuation: \"" + ch + "\"");
        }
    }
    function is_kw(kw) {
        var tok = input.peek();
        return tok && tok.type == "kw" && (!kw || tok.value == kw) && tok;
    }
    function skip_kw(kw) {
        if (is_kw(kw)) input.next();
        else input.croak("Expecting keyword: \"" + kw + "\"");
    }
    function delimited(start, stop, separator, parser) {
        let a = [], first = true;
        skip_punc(start);
        while (!input.eof()) {
            if (is_punc(stop)) break;
            if (first) first = false; else skip_punc(separator);
            if (is_punc(stop)) break;
            a.push(parser());
        }
        skip_punc(stop);
        return a;
    }
    function parse_call(func) {
        return {
            type: "call",
            func: func,
            args: delimited("(", ")", ",", parse_expression),
        };
    }
    function maybe_call(expr) {
        expr = expr();
        return is_punc("(") ? parse_call(expr) : expr;
    }
    function parse_toplevel() {
        const prog = [];
        while (!input.eof()) {
            prog.push(parse_expression());
            if (!input.eof()) skip_punc(';');
        }
        return {type: "prog", prog: prog};
    }
    function maybe_binary(left, my_prec) {
        var tok = is_op();
        if (tok) {
            var his_prec = PRECEDENCE[tok.value];
            if (his_prec > my_prec) {
                input.next();
                return maybe_binary({
                    type     : tok.value == "=" ? "assign" : "binary",
                    operator : tok.value,
                    left     : left,
                    right    : maybe_binary(parse_atom(), his_prec)
                }, my_prec);
            }
        }
        return left;
    }
    function parse_expression() {
        return maybe_call(() => {
            return maybe_binary(parse_atom(), 0);
        });
    }
    function parse_if() {
        skip_kw("if");
        const cond = parse_expression();
        if (!is_punc("{")) {
            skip_kw("then");
        }
        const then = parse_expression();
        var ret = {
            type: "if",
            cond: cond,
            then: then,
        };
        if (is_kw("else")) {
            input.next();
            ret.else = parse_expression();
        }
        return ret;
    }
    function parse_varname() {
        var tok = input.next();
        if (tok.type != "var") input.croak("Expecting variable name");
        return tok.value;
    }
    function parse_lambda() {
        return {
            type: "lambda",
            name: input.peek().type == "var" ? input.next().value : null,
            vars: delimited("(", ")", ",", parse_varname),
            body: parse_expression()
        };
    }
    function parse_bool() {
        return {
            type: "bool",
            value: input.next().value
        };
    }
    function parse_let() {
        skip_kw("let");
        if (input.peek().type === "var") {
            var name = input.next().value;
            var defs = delimited("(", ")", ",", parse_vardef);
            return {
                type: "call",
                func: {
                    type: "lambda",
                    name: name,
                    vars: defs.map(function(def){ return def.name }),
                    body: parse_expression(),
                },
                args: defs.map(function(def){ return def.def || FALSE })
            };
        }
        return {
            type: "let",
            vars: delimited("(", ")", ",", parse_vardef),
            body: parse_expression(),
        };
    }
    function parse_vardef() {
        var name = parse_varname(), def;
        if (is_op("=")) {
            input.next();
            def = parse_expression();
        }
        return { name: name, def: def };
    }
    function parse_atom() {
        return maybe_call(function(){
            if (is_punc("(")) {
                input.next();
                var exp = parse_expression();
                skip_punc(")");
                return exp;
            }
            if (is_punc("{")) {
                return parse_prog();
            }
            if (is_kw("let")) {
                return parse_let();
            }
            if (is_kw("if")) {
                return parse_if();
            }
            if (is_kw("true") || is_kw("false")) {
                return parse_bool();
            }
            if (is_kw("lambda") || is_kw("λ")) {
                input.next();
                return parse_lambda();
            }
            var tok = input.next();
            if (tok.type == "var" || tok.type == "num" || tok.type == "str") {
                return tok;
            }
            unexpected();
        });
    }
    function unexpected() {
        input.croak("Unexpected token: " + JSON.stringify(input.peek()));
    }
    function parse_prog() {
        const prog = delimited("{", "}", ";", parse_expression);
        if (prog.length == 0) return FALSE;
        if (prog.length == 1) return prog[0];
        return { type: "prog", prog: prog };
    }
}

function Environment(parent) {
    this.vars = Object.create(parent ? parent.vars : null);
    this.parent = parent;
}

Environment.prototype = {
    extend: function () {
        return new Environment(this);
    },
    lookup: function(name) {
        var scope = this;
        while(scope) {
            if (Object.prototype.hasOwnProperty.call(scope.vars, name)) {
                return scope;
            }
            scope = scope.parent;
        }
    },
    get: function(name) {
        if (name in this.vars) {
            return this.vars[name];
        }
        throw new Error(`Undefined variable ${name}`);
    },
    set: function(name, value) {
        var scope = this.lookup(name);
        if (!scope && this.parent) {
            throw new Error(`Undefined variable ${name}`);
        }
        return (scope || this).vars[name] = value;
    },
    def: function(name, value) {
        return this.vars[name] = value;
    }
}

function evaluate(exp, env) {
    switch(exp.type) {
        case "num":
        case "str":
        case "bool":
            return exp.value;
        case "var":
            return env.get(exp.value);
        case "assign":
            if (exp.left.type != "var") {
                throw new Error("Cannot assign to " + JSON.stringify(exp.left));
            }
            return env.set(exp.left.value, evaluate(exp.right, env));
        case "binary":
            return apply_op(exp.operator,
                            evaluate(exp.left, env),
                            evaluate(exp.right, env));
        case "lambda":
            return make_lambda(env, exp);
        case "let":
            exp.vars.forEach(function(v){
              var scope = env.extend();
              scope.def(v.name, v.def ? evaluate(v.def, env) : false);
              env = scope;
            });
            return evaluate(exp.body, env);
        case "if":
            var cond = evaluate(exp.cond, env);
            if (cond !== false) {
                return evaluate(exp.then, env);
            }
            return exp.else ? evaluate(exp.else, env) : false;
        case "prog":
            var val = false;
            exp.prog.forEach(function (exp) {
                val = evaluate(exp, env);
            });
            return val;
        case "call":
            var func = evaluate(exp.func, env);
            return func.apply(null, exp.args.map((arg) => {
                return evaluate(arg, env);
            }));
        default:
            throw new Error(`I don't know how to evaluate ${exp.type}`);
    }
}

function apply_op(op, a, b) {
    function num(x) {
        if (typeof x != "number") {
            throw new Error(`Expected number but got ${x.type}`);
        }
        return x;
    }
    function div(x) {
        if (num(x) == 0) {
            throw new Error("Divide by zero");
        }
        return x;
    }
    switch (op) {
      case "+"  : return num(a) + num(b);
      case "-"  : return num(a) - num(b);
      case "*"  : return num(a) * num(b);
      case "/"  : return num(a) / div(b);
      case "%"  : return num(a) % div(b);
      case "&&" : return a !== false && b;
      case "||" : return a !== false ? a : b;
      case "<"  : return num(a) < num(b);
      case ">"  : return num(a) > num(b);
      case "<=" : return num(a) <= num(b);
      case ">=" : return num(a) >= num(b);
      case "==" : return a === b;
      case "!=" : return a !== b;
    }
    throw new Error("Can't apply operator " + op);
}

function make_lambda(env, exp) {
    if (exp.name) {                    // these
        env = env.extend();            // lines
        env.def(exp.name, lambda);     // are
    }
    function lambda() {
        var names = exp.vars;
        var scope = env.extend();
        for (var i = 0; i < names.length; ++i) {
            scope.def(names[i], i < arguments.length ? arguments[i] : false);
        }
        return evaluate(exp.body, scope);
    }
    return lambda;
}



// const code = `
// fib = lambda (n) if n < 2 then n else fib(n - 1) + fib(n - 2);
// println("Hello World!");
// if a <= b {
//     print(", ");
// } else {
//     println("");
// }
// `;
// some test code here
// const code = "sum = lambda(x, y) x + y; print(sum(2, 3));";
const code = `
println(let loop (n = 10)
    if n > 0 then
        n + loop(n - 1)
    else
        0);
`;


// remember, parse takes a TokenStream which takes an InputStream
const ast = myparse(new InputStream(new Input(code)));
// create the global environment
const globalEnv = new Environment();

// define the "print" primitive function
globalEnv.def("print", function(txt){
  console.log(txt);
});
globalEnv.def("println", function(txt){
  console.log(txt);
});

// run the evaluator
evaluate(ast, globalEnv); // will print 5
