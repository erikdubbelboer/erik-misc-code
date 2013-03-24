
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


//<testonly>
module.exports = stringify;

