
var stringify = require('./stringify.js');


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

        if (typeof tests[i] != typeof test) {
          throw new Error('typeof mismatch');
        }

        if (typeof tests[i] == 'Object') {
          var fk = Object.keys(tests[i]);
          var rk = Object.keys(test);

          if (fk.length != rk.length) {
            throw new Error('object length mismatch');
          }

          for (var k = 0; k < fk.length; ++k) {
            if (tests[fk[k]] !== test[rk[k]]) {
              throw new Error('mismatch on propery ' + k);
            }
          }
        } else if (typeof tests[i] != 'undefined') {
          if (('' + tests[i]) != ('' + test)) {
            throw new Error('toString mismatch');
          }
        }
      });
    });
  })(i);
}

