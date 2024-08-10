!function() {
  const vert_shader = `
    struct upc {
      vec2 grid_pos;
      vec2 grid_size;
    };

    uniform upc pc;

    attribute vec2 pos;
    attribute vec4 i_pos;

    void main() {
      vec2 f_adj = pos * 0.0001; // avoid one-pixel gaps

      vec2 p = pos * i_pos.zw;

      vec2 f_pos = (p + i_pos.xy - pc.grid_pos) / pc.grid_size; 
      gl_Position = vec4(f_pos + f_adj, 0, 1);
    }
  `;
  const frag_shader = `
    void main() {
      gl_FragColor = vec4(1, 0, 0, 1);
    }
  `;

  const canvas = document.getElementById("casein-canvas");
  const gl = canvas.getContext("webgl");
  const ext = gl.getExtension("ANGLE_instanced_arrays");

  const prog = gl.createProgram();

  const vert = gl.createShader(gl.VERTEX_SHADER);
  gl.shaderSource(vert, vert_shader);
  gl.compileShader(vert);
  if (!gl.getShaderParameter(vert, gl.COMPILE_STATUS)) {
    console.error(gl.getShaderInfoLog(vert));
  }
  gl.attachShader(prog, vert);

  const frag = gl.createShader(gl.FRAGMENT_SHADER);
  gl.shaderSource(frag, frag_shader);
  gl.compileShader(frag);
  if (!gl.getShaderParameter(frag, gl.COMPILE_STATUS)) {
    console.error(gl.getShaderInfoLog(frag));
  }
  gl.attachShader(prog, frag);

  gl.linkProgram(prog);
  if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
    console.error(gl.getProgramInfoLog(prog));
  }

  gl.useProgram(prog);

  v_array = new Float32Array([ 1, 1, -1, 1, 1, -1, 1, -1, -1, 1, -1, -1 ]);
  v_buf = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, v_buf);
  gl.bufferData(gl.ARRAY_BUFFER, v_array, gl.STATIC_DRAW);
  gl.enableVertexAttribArray(0);
  gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 0, 0);

  const i_stride = 80;

  i_buf = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, i_buf);
  gl.enableVertexAttribArray(1);
  gl.vertexAttribPointer(1, 4, gl.FLOAT, false, i_stride, 0);
  ext.vertexAttribDivisorANGLE(1, 1);

  const u_pos = gl.getUniformLocation(prog, "pc.grid_pos");
  const u_size = gl.getUniformLocation(prog, "pc.grid_size");

  gl.uniform2f(u_pos, 0, 0);
  gl.uniform2f(u_size, 8, 8);

  var i_count = 0;
  function draw() {
    if (i_count > 0) {
      gl.clear(gl.COLOR_BUFFER_BIT);
      gl.viewport(0, 0, canvas.width, canvas.height);

      ext.drawArraysInstancedANGLE(gl.TRIANGLES, 0, 6, i_count);
    }

    requestAnimationFrame(draw);
  }

  leco_imports.quack = {
    clear_colour : (r, g, b, a) => gl.clearColor(r, g, b, a),
    bind_instances : (count, ptr, sz) => {
      const data = new DataView(leco_exports.memory.buffer, ptr, sz * i_stride);
      gl.bindBuffer(gl.ARRAY_BUFFER, i_buf);
      gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
      i_count = count;
    },
    start : () => requestAnimationFrame(draw),
  };
}();
