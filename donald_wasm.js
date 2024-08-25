!function() {
  const objs = quack_gl();
  const { gl, ext, canvas, u } = objs;

  const buf = quack_buffer(objs);
  const txt = quack_texture(objs);

  var i_count = 0;
  function draw() {
    if (i_count > 0) {
      gl.clear(gl.COLOR_BUFFER_BIT);
      gl.viewport(0, 0, canvas.width, canvas.height);

      ext.drawArraysInstancedANGLE(gl.TRIANGLES, 0, 6, i_count);
    }

    requestAnimationFrame(draw);
  }

  leco_imports.quack_donald = {
    clear_colour : (r, g, b, a) => gl.clearColor(r, g, b, a),
    bind_instances : (count, ptr, sz) => {
      const data = new DataView(leco_exports.memory.buffer, ptr, sz * buf.stride);
      gl.bindBuffer(gl.ARRAY_BUFFER, buf.buf);
      gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
      i_count = count;
    },
    load_texture : (ptr, sz) => quack_load_texture(objs, txt, ptr, sz),
    set_grid : (px, py, sx, sy) => {
      gl.uniform2f(u.pos, px, py);
      gl.uniform2f(u.size, sx, sy);
    },
    start : () => requestAnimationFrame(draw),
  };
}();
