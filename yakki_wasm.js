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
    update_buf : (i, ptr, sz) => {
      const { gl } = objs;
      var { buf, stride } = bufs[i - 1];
      var data = vaselin_toarr(ptr, sz * stride);
      gl.bindBuffer(gl.ARRAY_BUFFER, buf);
      gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
    },
    clear_canvas : (r, g, b, a) => {
      const { gl } = objs;
      gl.clearColor(r, g, b, a);
      quack_clear(objs);
    },
    run_batch : (b, i, qty, first) => {
      const { gl, ext } = objs;
      gl.bindBuffer(gl.ARRAY_BUFFER, bufs[b - 1].buf);
      gl.bindTexture(gl.TEXTURE_2D, txts[i - 1]);
      ext.drawArraysInstancedANGLE(gl.TRIANGLES, 0, 6, qty);
    },
  };
}();
