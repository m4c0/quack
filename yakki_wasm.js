!function() {
  var objs = {};
  var bufs = [];
  var txts = [];

  leco_imports.quack_yakki = {
    alloc_text : (ptr, sz) => {
      var t = quack_texture(objs);
      quack_load_texture(objs, t, ptr, sz);
      return txts.push(t);
    },
  };
}();
