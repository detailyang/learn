/*
* @Author: detailyang
* @Date:   2016-04-25 00:18:09
* @Last Modified by:   detailyang
* @Last Modified time: 2016-04-25 03:23:19
*/

'use strict';

export const lineBreak = /\r\n?|\n|\u2028|\u2029/;
export const lineBreakG = new RegExp(lineBreak.source, "g");

export default class Tokenizer {
    constructor(input, options) {
        this.options = options;
        this.input = input;
        this.pos = 0;
        this.line = 0;
        this.lineStart = 0;
        this.tokens = [];
        this.comments = [];
    }

    next() {
        while(this.pos < this.input.length) {
            const ch = this.input.charCodeAt(this.pos);
            switch (ch) {
                case ' ':
                    this.pos++;
                    break;
                case 47:
                    switch (this.input.charCodeAt(this.pos + 1)) {
                        case 42:
                            this.skipBlockComment();
                            break;
                        case 47:
                            this.skipLineComment(2);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    this.pos++;
                    break;
            }
        }
    }

    skipBlockComment() {
        const start = this.pos;
        const end = this.input.indexOf("*/", start + 2);
        if (end === -1) {
            this.raise(start, "Unterminated comment");
        }

        this.pos = end + 2;
        lineBreakG.lastIndex = start;
        let match = lineBreakG.exec(this.input);
        while (match && match.index < this.pos) {
            ++this.line;
            this.lineStart = match.index + match[0].length;
            let match = lineBreakG.exec(this.input);
        }

        this.pushComment(true, this.input.slice(start+2, end), start, end + 2);
    }

    pushComment(block, text, start, end) {
        const comment = {
          type: block ? "CommentBlock" : "CommentLine",
          value: text,
          start: start,
          end: end,
        };

        this.tokens.push(comment);
        this.comments.push(comment);
    }
}