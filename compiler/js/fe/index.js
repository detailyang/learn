/*
* @Author: detailyang
* @Date:   2016-04-25 02:40:20
* @Last Modified by:   detailyang
* @Last Modified time: 2016-04-30 17:23:33
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
        const keywords = " if then else lambda λ true false ";
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

const code = `
# this is a comment

println("Hello World!");

println(2 + 3 * 4);

# functions are introduced with lambda or λ
fib = lambda (n) if n < 2 then n else fib(n - 1) + fib(n - 2);

println(fib(15));

print-range = λ(a, b)             # λ is synonym to lambda
                if a <= b then {  # then here is optional as you can see below
                  print(a);
                  if a + 1 <= b {
                    print(", ");
                    print-range(a + 1, b);
                  } else println("");        # newline
                };
print-range(1, 5);
`;

const is = new InputStream(new Input(code));
const tokens = [];
while (!is.eof()) {
    tokens.push(is.next());
}
console.log(tokens);