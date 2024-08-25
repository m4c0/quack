!function() {
  var objs = {};
  var bufs = [];
  var txts = [];

  leco_imports.quack_yakki = {
    start : () => objs = quack_gl(),
    alloc_text : (ptr, sz) => {
      var t = quack_texture(objs);
      quack_load_texture(objs, t, ptr, sz);
      return txts.push(t);
    },
    alloc_buf : () => bufs.push(quack_buffer(objs)),
  };
}();
