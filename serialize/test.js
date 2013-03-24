
var assert = require('assert');


function stringify(o) {
  if (typeof o === 'undefined') {
    return 'undefined';
  }
  if (o instanceof Array) {
    // Array.map is smart about array's with holes in them
    // so use it when possible (not supported in IE 6,7).
    if (o.map) {
      return '[' + o.map(stringify).join(', ') + ']';
    }

    var r = '[';

    for (var i = 0; i < o.length; ++i) {
      r += stringify(o[i]);
     
      if (i < (o.length - 1)) {
        r += ', ';
      }
    }

    return r + ']';
  }
  if (o instanceof Date) {
    return 'new Date(' + o.getTime() + ')';
  }
  if (typeof o === 'object') {
    var kv = [];
     
    for (var p in o) {
      if (o.hasOwnProperty(p)) {
      kv.push(p + ': ' + stringify(o[p]));
      }
    }
     
    return '{\n' + kv.join(',\n') + '\n}';
  }
  if (typeof o === 'string') {
    return '"' + o
      .replace(/\\/g , '\\\\')
      .replace(/\u0008/g, '\\b') // Backspace.
      .replace(/\t/g , '\\t')
      .replace(/\n/g , '\\n')
      .replace(/\f/g , '\\f')
      .replace(/\r/g , '\\r')
      .replace(/"/g , '\\"')
      + '"';
  }
   
  return o.toString();
}


var tests = [
  1,
  'a',
  [1, 2],
  {a: 1, b: 2},
  [{a: 1}, 2],
  new Date(),
  0.999,
  'a\\b\nc\td"e\'f',
  undefined,
  [undefined, 2],
  [1],
  [],
  {},
  function(a) { return 1 + a; },
  function(n) { return Math.floor(n); }
];

for (var i = 0; i < tests.length; ++i) {
  (function(i) {
    describe('test ' + i, function() {
      it('should serialize: ' + ((tests[i] == undefined) ? 'undefined' : tests[i].toString()), function() {
        var test;
        eval('test = ' + stringify(tests[i]));

        assert.equal(JSON.stringify(test), JSON.stringify(tests[i]));
      });
    });
  })(i);
}

