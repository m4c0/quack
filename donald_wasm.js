!function() {
  const objs = quack_gl();
  const { gl, ext, canvas, u } = objs;

  const i_stride = 80;

  i_buf = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, i_buf);
  gl.enableVertexAttribArray(1);
  gl.vertexAttribPointer(1, 4, gl.FLOAT, false, i_stride, 0);
  ext.vertexAttribDivisorANGLE(1, 1);
  gl.enableVertexAttribArray(2);
  gl.vertexAttribPointer(2, 4, gl.FLOAT, false, i_stride, 32);
  ext.vertexAttribDivisorANGLE(2, 1);
  gl.enableVertexAttribArray(3);
  gl.vertexAttribPointer(3, 4, gl.FLOAT, false, i_stride, 16);
  ext.vertexAttribDivisorANGLE(3, 1);
  gl.enableVertexAttribArray(4);
  gl.vertexAttribPointer(4, 4, gl.FLOAT, false, i_stride, 48);
  ext.vertexAttribDivisorANGLE(4, 1);
  gl.enableVertexAttribArray(5);
  gl.vertexAttribPointer(5, 4, gl.FLOAT, false, i_stride, 64);
  ext.vertexAttribDivisorANGLE(5, 1);

  const txt = gl.createTexture();
  gl.activeTexture(gl.TEXTURE0);
  gl.bindTexture(gl.TEXTURE_2D, txt);
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array([255, 0, 255, 255]));

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
      const data = new DataView(leco_exports.memory.buffer, ptr, sz * i_stride);
      gl.bindBuffer(gl.ARRAY_BUFFER, i_buf);
      gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
      i_count = count;
    },
    load_texture : (ptr, sz) => {
      const img = new Image();
      img.onload = () => {
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
      };
      img.src = vaselin_tostr(ptr, sz);
    },
    set_grid : (px, py, sx, sy) => {
      gl.uniform2f(u.pos, px, py);
      gl.uniform2f(u.size, sx, sy);
    },
    start : () => requestAnimationFrame(draw),
  };
}();
